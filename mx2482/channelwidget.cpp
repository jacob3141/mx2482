///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    This file is part of QJackAudio.                                       //
//    Copyright (C) 2014 Jacob Dawid, jacob@omg-it.works                     //
//                                                                           //
//    QJackAudio is free software: you can redistribute it and/or modify     //
//    it under the terms of the GNU General Public License as published by   //
//    the Free Software Foundation, either version 3 of the License, or      //
//    (at your option) any later version.                                    //
//                                                                           //
//    QJackAudio is distributed in the hope that it will be useful,          //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of         //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          //
//    GNU General Public License for more details.                           //
//                                                                           //
//    You should have received a copy of the GNU General Public License      //
//    along with QJackAudio. If not, see <http://www.gnu.org/licenses/>.     //
//                                                                           //
//    It is possible to obtain a closed-source license of QJackAudio.        //
//    If you're interested, contact me at: jacob@omg-it.works                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// Own includes
#include "channelwidget.h"
#include "ui_channelwidget.h"

// QJackAudio includes
#include <QSampleBuffer>
#include <QUnits>

ChannelWidget::ChannelWidget(int channelNumber, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelWidget)
{
    ui->setupUi(this);

    // Set channel number in UI
    ui->channelNumberLabel->setText(QString("%1").arg(channelNumber));

    // Create JACK ports
    QJackClient *jackClient = QJackClient::instance();
    _channelIn  = jackClient->registerAudioInPort (QString("ch%1_in")       .arg(channelNumber));
    _auxSend    = jackClient->registerAudioOutPort(QString("ch%1_aux_send") .arg(channelNumber));
    _auxReturn  = jackClient->registerAudioInPort (QString("ch%1_aux_ret")  .arg(channelNumber));
    _channelOut = jackClient->registerAudioOutPort(QString("ch%1_out")      .arg(channelNumber));

    // Create input and fader stage amplifiers
    _inputStage = new QAmplifier();
    _inputStage->setGain(ui->gainDial->value());

    _faderStage = new QAmplifier();
    _faderStage->setGain(ui->volumeVerticalSlider->value());

    // Create aux pre and post amplifiers
    _auxPre = new QAmplifier();
    _auxPre->setGain(ui->auxSendDial->value());
    _auxPost = new QAmplifier();
    _auxPost->setGain(ui->auxReturnDial->value());

    // Create equalizer
    _equalizer = new QEqualizer(256, 128);

    // Create equalizer controls
    _lowsEqControl = _equalizer->createEqualizerControl(QEqualizerControl::LowShelf);
    _lowsEqControl->setAmount(ui->loDial->value());
    _lowsEqControl->setControlFrequency(ui->loFreqDial->value());
    _midsEqControl = _equalizer->createEqualizerControl(QEqualizerControl::Band);
    _midsEqControl->setAmount(ui->midDial->value());
    _midsEqControl->setControlFrequency(ui->midFreqDial->value());
    _highsEqControl = _equalizer->createEqualizerControl(QEqualizerControl::HighShelf);
    _highsEqControl->setAmount(ui->hiDial->value());
    _highsEqControl->setControlFrequency(6000);
    _equalizer->update();

    // Connect UI elements to widgets
    connect(ui->gainDial, SIGNAL(valueChanged(int)), _inputStage, SLOT(setGain(int)));
    connect(ui->volumeVerticalSlider, SIGNAL(valueChanged(int)), _faderStage, SLOT(setGain(int)));

    connect(ui->loDial, SIGNAL(valueChanged(int)), _lowsEqControl, SLOT(setAmount(int)));
    connect(ui->loFreqDial, SIGNAL(valueChanged(int)), _lowsEqControl, SLOT(setControlFrequency(int)));

    connect(ui->midDial, SIGNAL(valueChanged(int)), _midsEqControl, SLOT(setAmount(int)));
    connect(ui->midFreqDial, SIGNAL(valueChanged(int)), _midsEqControl, SLOT(setControlFrequency(int)));

    connect(ui->hiDial, SIGNAL(valueChanged(int)), _highsEqControl, SLOT(setAmount(int)));
}

ChannelWidget::~ChannelWidget()
{
    delete ui;
}

void ChannelWidget::process(QSampleBuffer targetSampleBuffer)
{
    // Get the hardware input buffer for this channel input
    QSampleBuffer inputSampleBuffer = _channelIn->sampleBuffer();

    // Copy all data to a memory buffer
    inputSampleBuffer.copyTo(targetSampleBuffer);

    // Process input stage amplifier
    _inputStage->process(targetSampleBuffer);

    // Check if EQ is activated and process
    if(ui->equalizerOnPushButton->isChecked()) {
        _equalizer->process(targetSampleBuffer);
    }

    // Check if aux send/return is activated and process
    if(ui->auxOnPushButton->isChecked()) {
        // Attenuate signal
        _auxPre->process(targetSampleBuffer);
        // Send signal
        targetSampleBuffer.copyTo(_auxSend->sampleBuffer());
        // Take received signal
        _auxReturn->sampleBuffer().copyTo(targetSampleBuffer);
        // Attenuate signal
        _auxPost->process(targetSampleBuffer);
    }

    // Process fader stage amplifier
    _faderStage->process(targetSampleBuffer);

    // Determine peak and convert to dB.
    _peakDb = QUnits::linearToDb(targetSampleBuffer.peak());

    // Transfer data to channel direct out.
    targetSampleBuffer.copyTo(_channelOut->sampleBuffer());
}

void ChannelWidget::updateInterface()
{
    ui->progressBar->setValue((int)_peakDb);
}

double ChannelWidget::panorama()
{
    return (double)ui->panDial->value() / 100.0;
}

bool ChannelWidget::isInSubGroup12()
{
    return ui->subgroup12PushButton->isChecked();
}

bool ChannelWidget::isInSubGroup34()
{
    return ui->subgroup34PushButton->isChecked();
}

bool ChannelWidget::isInSubGroup56()
{
    return ui->subgroup56PushButton->isChecked();
}

bool ChannelWidget::isInSubGroup78()
{
    return ui->subgroup78PushButton->isChecked();
}

bool ChannelWidget::isMuted()
{
    return ui->mutePushButton->isChecked();
}

bool ChannelWidget::isSoloed()
{
    return ui->soloPushButton->isChecked();
}

bool ChannelWidget::isOnMain()
{
    return ui->mainPushButton->isChecked();
}
