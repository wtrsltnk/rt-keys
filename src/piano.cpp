/*
  ZynAddSubFX - a software synthesizer

  keyboard.cpp - Keyboard widget for running notes to the synth
  Copyright (C) 2014 Wouter Saaltink
  Author: Wouter Saaltink

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2 or later) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/
#include "piano.h"
#include "ui_piano.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTextDocument>
#include <QTextOption>
#include <iostream>
#include "rtoutput.h"

using namespace std;

int Piano::KeyWidth = 24;
int Piano::KeyHeight = 72;

unsigned char Piano::_characterNoteMapping[128] = { 0 };

Key::Key(Octave* o, unsigned char n)
    : QGraphicsPolygonItem(o), _parent(o), _note(n), _isEnabled(true)
{
    this->setAcceptHoverEvents(true);
    QPolygon p;
    if (n % 12 == 0 || n % 12 == 2 || n % 12 == 4 || n % 12 == 5 || n % 12 == 7 || n % 12 == 9 || n % 12 == 11)
    {
        int a = 0;
        if (n % 12 == 2) a = 1;
        if (n % 12 == 4) a = 2;
        if (n % 12 == 5) a = 3;
        if (n % 12 == 7) a = 4;
        if (n % 12 == 9) a = 5;
        if (n % 12 == 11) a = 6;
        p << QPoint(a * Piano::KeyWidth + 0, 0)
          << QPoint(a * Piano::KeyWidth + Piano::KeyWidth, 0)
          << QPoint(a * Piano::KeyWidth + Piano::KeyWidth, Piano::KeyHeight)
          << QPoint(a * Piano::KeyWidth + 0, Piano::KeyHeight);
        this->setBrush(QBrush(Qt::white));
        this->setZValue(1);
        this->_isSharp = false;
        QString txt = QString("%1%2").arg(QString('A' + a), QString::number(o->_octave));
        QGraphicsTextItem* t = new QGraphicsTextItem(txt);
        t->setPos(a * Piano::KeyWidth, Piano::KeyHeight);
        t->setDefaultTextColor(Qt::white);
        o->addToGroup(t);
    }
    else
    {
        int a = 0;
        if (n % 12 == 3) a = 1;
        if (n % 12 == 6) a = 3;
        if (n % 12 == 8) a = 4;
        if (n % 12 == 10) a = 5;
        p << QPoint(a * Piano::KeyWidth + ((Piano::KeyWidth/3) * 2), 0)
          << QPoint(a * Piano::KeyWidth + ((Piano::KeyWidth/3) * 2) + ((Piano::KeyWidth/3) * 2), 0)
          << QPoint(a * Piano::KeyWidth + ((Piano::KeyWidth/3) * 2) + ((Piano::KeyWidth/3) * 2), Piano::KeyHeight * 0.6)
          << QPoint(a * Piano::KeyWidth + ((Piano::KeyWidth/3) * 2), Piano::KeyHeight * 0.6);
        this->setBrush(QBrush(Qt::black));
        this->setZValue(2);
        this->_isSharp = true;
    }
    this->setPolygon(p);
}

Key::~Key()
{ }

void Key::SetOn(bool on)
{
    if (this->_isEnabled)
    {
        if (on)
        {
            if (this->_isSharp)
                this->setBrush(QBrush(QColor::fromRgb(0, 48, 64)));
            else
                this->setBrush(QBrush(QColor::fromRgb(0, 172, 229)));
        }
        else
        {
            if (this->_isSharp)
                this->setBrush(QBrush(Qt::black));
            else
                this->setBrush(QBrush(Qt::white));
        }
        this->update();
    }
}

void Key::SetEnabled(bool enabled)
{
    this->_isEnabled = enabled;
    if (this->_isEnabled == false)
    {
        if (this->_isSharp)
            this->setBrush(QBrush(Qt::gray));
        else
            this->setBrush(QBrush(Qt::lightGray));
    }
    else
    {
        if (this->_isSharp)
            this->setBrush(QBrush(Qt::black));
        else
            this->setBrush(QBrush(Qt::white));
    }
    this->update();
}

void Key::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    this->SetOn(true);
}

void Key::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    this->SetOn(false);
}

Octave::Octave(unsigned char o)
    : QGraphicsItemGroup(), _octave(o)
{
    this->setHandlesChildEvents(false);
    for (int i = 0; i < 12; i++)
    {
        if (o * 12 + i < 128)
        {
            this->_keys[i] = new Key(this, o * 12 + i);
            this->addToGroup(this->_keys[i]);
        }
        else
            this->_keys[i] = 0;
    }
}

Octave::~Octave()
{ }

Piano::Piano(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Keyboard), _sendnotes(false)
{
    ui->setupUi(this);

    this->ui->graphicsView->setMouseTracking(true);
    this->ui->graphicsView->viewport()->setMouseTracking(true);

    this->_scene = new QGraphicsScene();
    for (int i = 0; i < MAX_OCTAVES; i++)
    {
        this->_octaves[i] = new Octave(i);
        this->_scene->addItem(this->_octaves[i]);
        this->_octaves[i]->setPos(Piano::KeyWidth * 7 * i, 0);
    }
    this->_scene->setBackgroundBrush(QBrush(QColor::fromRgb(53, 53, 53)));

    this->ui->graphicsView->setScene(this->_scene);

    for (int i = 0; i < 128; i++)
        this->_selectedNotes[i] = false;

    int startoctave = 60;
    Piano::_characterNoteMapping['Z'] = startoctave+0;
     Piano::_characterNoteMapping['S'] = startoctave+1;
    Piano::_characterNoteMapping['X'] = startoctave+2;
     Piano::_characterNoteMapping['D'] = startoctave+3;
    Piano::_characterNoteMapping['C'] = startoctave+4;
    Piano::_characterNoteMapping['V'] = startoctave+5;
     Piano::_characterNoteMapping['G'] = startoctave+6;
    Piano::_characterNoteMapping['B'] = startoctave+7;
     Piano::_characterNoteMapping['H'] = startoctave+8;
    Piano::_characterNoteMapping['N'] = startoctave+9;
     Piano::_characterNoteMapping['J'] = startoctave+10;
    Piano::_characterNoteMapping['M'] = startoctave+11;
    Piano::_characterNoteMapping[','] = startoctave+12;
     Piano::_characterNoteMapping['L'] = startoctave+13;
    Piano::_characterNoteMapping['.'] = startoctave+14;
     Piano::_characterNoteMapping[';'] = startoctave+15;
    Piano::_characterNoteMapping['/'] = startoctave+16;
}

Piano::~Piano()
{
    delete ui;
}

void Piano::SelectCharacter(char c, bool on)
{
    unsigned char note = Piano::_characterNoteMapping[c];
    if (note >= 0 && note < 128)
    {
        int key = note % 12;
        int octave = (note - key) / 12;
        if (octave < MAX_OCTAVES && key < 12)
            this->_octaves[(note - (note % 12)) / 12]->_keys[note % 12]->SetOn(on);
        if (this->_selectedNotes[note] != on)
        {
            if (on)
                this->noteOn(note, 100);
            else
                this->noteOff(note);

            this->_selectedNotes[note] = on;
        }
    }
}

void Piano::HighlightNote(unsigned char note, bool on)
{
    if (note >= 0 && note < 128)
    {
        int key = note % 12;
        int octave = (note - key) / 12;
        if (octave < MAX_OCTAVES && key < 12)
            this->_octaves[(note - (note % 12)) / 12]->_keys[note % 12]->SetOn(on);
    }
}

void Piano::EnableNote(unsigned char note, bool enabled)
{
    if (note >= 0 && note < 128)
    {
        int key = note % 12;
        int octave = (note - key) / 12;
        if (octave < MAX_OCTAVES && key < 12)
            this->_octaves[octave]->_keys[key]->SetEnabled(enabled);
    }
}

void Piano::SetNoteColor(unsigned char note, const QColor& color)
{
    if (note >= 0 && note < 128)
    {
        int key = note % 12;
        int octave = (note - key) / 12;
        if (octave < MAX_OCTAVES && key < 12)
            this->_octaves[octave]->_keys[key]->setPen(QPen(color));
    }
}

void Piano::keyPressEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat() == false)
        this->SelectCharacter(char(event->key()), true);
}

void Piano::keyReleaseEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat() == false)
        this->SelectCharacter(char(event->key()), false);
}
