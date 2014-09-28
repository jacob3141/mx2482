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
#include "mainmixerwidget.h"
#include "ui_mainmixerwidget.h"
#include "aboutdialog.h"

// Qt includes
#include <QFontDatabase>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QJsonDocument>

MainMixerWidget::MainMixerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainMixerWidget)
{
    ui->setupUi(this);

    connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(updateInterface()));
    _updateTimer.setInterval(20);
    _updateTimer.setSingleShot(false);
    _updateTimer.start();

    QFontDatabase::addApplicationFont(":/fonts/FreePixel.ttf");
    QFont font("Free Pixel", 24);
    font.setStyleStrategy(QFont::NoAntialias);
    ui->displayLabel->setFont(font);

    QJackClient *jackClient = QJackClient::instance();
    _subGroup1Out = jackClient->registerAudioOutPort("subgroup1_out");
    _subGroup2Out = jackClient->registerAudioOutPort("subgroup2_out");
    _subGroup3Out = jackClient->registerAudioOutPort("subgroup3_out");
    _subGroup4Out = jackClient->registerAudioOutPort("subgroup4_out");
    _subGroup5Out = jackClient->registerAudioOutPort("subgroup5_out");
    _subGroup6Out = jackClient->registerAudioOutPort("subgroup6_out");
    _subGroup7Out = jackClient->registerAudioOutPort("subgroup7_out");
    _subGroup8Out = jackClient->registerAudioOutPort("subgroup8_out");

    _mainLeftOut = jackClient->registerAudioOutPort("main_out_1");
    _mainRightOut = jackClient->registerAudioOutPort("main_out_2");

    _subgroup1FaderStage = new QAmplifier();
    _subgroup2FaderStage = new QAmplifier();
    _subgroup3FaderStage = new QAmplifier();
    _subgroup4FaderStage = new QAmplifier();
    _subgroup5FaderStage = new QAmplifier();
    _subgroup6FaderStage = new QAmplifier();
    _subgroup7FaderStage = new QAmplifier();
    _subgroup8FaderStage = new QAmplifier();
    _main1FaderStage = new QAmplifier();
    _main2FaderStage = new QAmplifier();

    _subgroup1FaderStage->setGain(ui->subgroup1VolumeVerticalSlider->value());
    _subgroup2FaderStage->setGain(ui->subgroup2VolumeVerticalSlider->value());
    _subgroup3FaderStage->setGain(ui->subgroup3VolumeVerticalSlider->value());
    _subgroup4FaderStage->setGain(ui->subgroup4VolumeVerticalSlider->value());
    _subgroup5FaderStage->setGain(ui->subgroup5VolumeVerticalSlider->value());
    _subgroup6FaderStage->setGain(ui->subgroup6VolumeVerticalSlider->value());
    _subgroup7FaderStage->setGain(ui->subgroup7VolumeVerticalSlider->value());
    _subgroup8FaderStage->setGain(ui->subgroup8VolumeVerticalSlider->value());

    _main1FaderStage->setGain(ui->main1VolumeVerticalSlider->value());
    _main2FaderStage->setGain(ui->main2VolumeVerticalSlider->value());

    connect(ui->subgroup1VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup1FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup2VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup2FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup3VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup3FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup4VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup4FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup5VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup5FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup6VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup6FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup7VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup7FaderStage, SLOT(setGain(int)));
    connect(ui->subgroup8VolumeVerticalSlider, SIGNAL(valueChanged(int)), _subgroup8FaderStage, SLOT(setGain(int)));

    connect(ui->main1VolumeVerticalSlider, SIGNAL(valueChanged(int)), _main1FaderStage, SLOT(setGain(int)));
    connect(ui->main2VolumeVerticalSlider, SIGNAL(valueChanged(int)), _main2FaderStage, SLOT(setGain(int)));
}

MainMixerWidget::~MainMixerWidget()
{
    delete ui;
}


void MainMixerWidget::registerChannel(int i, ChannelWidget *channelWidget)
{
    _registeredChannels.insert(i, channelWidget);
}

void MainMixerWidget::process()
{
    // Obtaining sample buffers
    QSampleBuffer subgroup1SampleBuffer = _subGroup1Out->sampleBuffer();
    QSampleBuffer subgroup2SampleBuffer = _subGroup2Out->sampleBuffer();
    QSampleBuffer subgroup3SampleBuffer = _subGroup3Out->sampleBuffer();
    QSampleBuffer subgroup4SampleBuffer = _subGroup4Out->sampleBuffer();
    QSampleBuffer subgroup5SampleBuffer = _subGroup5Out->sampleBuffer();
    QSampleBuffer subgroup6SampleBuffer = _subGroup6Out->sampleBuffer();
    QSampleBuffer subgroup7SampleBuffer = _subGroup7Out->sampleBuffer();
    QSampleBuffer subgroup8SampleBuffer = _subGroup8Out->sampleBuffer();

    QSampleBuffer main1SampleBuffer = _mainLeftOut->sampleBuffer();
    QSampleBuffer main2SampleBuffer = _mainRightOut->sampleBuffer();

    // Clearing buffers (since we are going to sum up signals
    subgroup1SampleBuffer.clear();
    subgroup2SampleBuffer.clear();
    subgroup3SampleBuffer.clear();
    subgroup4SampleBuffer.clear();
    subgroup5SampleBuffer.clear();
    subgroup6SampleBuffer.clear();
    subgroup7SampleBuffer.clear();
    subgroup8SampleBuffer.clear();

    main1SampleBuffer.clear();
    main2SampleBuffer.clear();

    // Find out whether any of the channels is soloed
    bool soloActive = false;
    foreach(ChannelWidget *channelWidget, _registeredChannels) {
        if(channelWidget->isSoloed()) {
            soloActive = true;
        }
    }

    // Find out whether any of the subgroups are soloed
    bool subgroupSoloActive = false;
    if(ui->subgroup1SoloPushButton->isChecked()
    || ui->subgroup2SoloPushButton->isChecked()
    || ui->subgroup3SoloPushButton->isChecked()
    || ui->subgroup4SoloPushButton->isChecked()
    || ui->subgroup5SoloPushButton->isChecked()
    || ui->subgroup6SoloPushButton->isChecked()
    || ui->subgroup7SoloPushButton->isChecked()
    || ui->subgroup8SoloPushButton->isChecked()) {
        subgroupSoloActive = true;
    }

    // Routing channels to subgroups and main
    foreach(ChannelWidget *channelWidget, _registeredChannels) {
        // Create a temporary memory buffer, so we do not alter the sample in the input buffer,
        // which may effect other applications connected to the same input.
        QSampleBuffer sampleBuffer = QSampleBuffer::createMemoryAudioBuffer(QJackClient::instance()->bufferSize());

        // Do the processing for the channel
        channelWidget->process(sampleBuffer);

        // If the channel is not muted, apply to subgroups and main.
        if(!channelWidget->isMuted() && (!soloActive || channelWidget->isSoloed())) {
            double panorama = channelWidget->panorama();
            if(channelWidget->isInSubGroup12()) {
                sampleBuffer.addTo(subgroup1SampleBuffer, 1.0 - panorama);
                sampleBuffer.addTo(subgroup2SampleBuffer,       panorama);
            }

            if(channelWidget->isInSubGroup34()) {
                sampleBuffer.addTo(subgroup3SampleBuffer, 1.0 - panorama);
                sampleBuffer.addTo(subgroup4SampleBuffer,       panorama);
            }

            if(channelWidget->isInSubGroup56()) {
                sampleBuffer.addTo(subgroup5SampleBuffer, 1.0 - panorama);
                sampleBuffer.addTo(subgroup6SampleBuffer,       panorama);
            }

            if(channelWidget->isInSubGroup78()) {
                sampleBuffer.addTo(subgroup7SampleBuffer, 1.0 - panorama);
                sampleBuffer.addTo(subgroup8SampleBuffer,       panorama);
            }

            if(channelWidget->isOnMain()) {
                sampleBuffer.addTo(main1SampleBuffer, 1.0 - panorama);
                sampleBuffer.addTo(main2SampleBuffer,       panorama);
            }
        }

        sampleBuffer.releaseMemoryBuffer();
    }

    // Route subgroups through faders
    _subgroup1FaderStage->process(subgroup1SampleBuffer);
    _subgroup2FaderStage->process(subgroup2SampleBuffer);
    _subgroup3FaderStage->process(subgroup3SampleBuffer);
    _subgroup4FaderStage->process(subgroup4SampleBuffer);
    _subgroup5FaderStage->process(subgroup5SampleBuffer);
    _subgroup6FaderStage->process(subgroup6SampleBuffer);
    _subgroup7FaderStage->process(subgroup7SampleBuffer);
    _subgroup8FaderStage->process(subgroup8SampleBuffer);

    // Routing subgroups to main

    if(!ui->subgroup1MutePushButton->isChecked() && ui->subgroup1MainPushButton->isChecked() && (!subgroupSoloActive || ui->subgroup1SoloPushButton->isChecked())) {
        subgroup1SampleBuffer.addTo(main1SampleBuffer);
    }
    if(!ui->subgroup2MutePushButton->isChecked() && ui->subgroup2MainPushButton->isChecked() && (!subgroupSoloActive || ui->subgroup2SoloPushButton->isChecked())) {
        subgroup2SampleBuffer.addTo(main2SampleBuffer);
    }
    if(!ui->subgroup3MutePushButton->isChecked() && ui->subgroup3MainPushButton->isChecked() && (!subgroupSoloActive || ui->subgroup3SoloPushButton->isChecked())) {
        subgroup3SampleBuffer.addTo(main1SampleBuffer);
    }
    if(!ui->subgroup4MutePushButton->isChecked() && ui->subgroup4MainPushButton->isChecked() && (!subgroupSoloActive || ui->subgroup4SoloPushButton->isChecked())) {
        subgroup4SampleBuffer.addTo(main2SampleBuffer);
    }
    if(!ui->subgroup5MutePushButton->isChecked() && ui->subgroup5MainPushButton->isChecked() && (!subgroupSoloActive || ui->subgroup5SoloPushButton->isChecked())) {
        subgroup5SampleBuffer.addTo(main1SampleBuffer);
    }
    if(!ui->subgroup6MutePushButton->isChecked() && ui->subgroup6MainPushButton->isChecked() && (!subgroupSoloActive || ui->subgroup6SoloPushButton->isChecked())) {
        subgroup6SampleBuffer.addTo(main2SampleBuffer);
    }
    if(!ui->subgroup7MutePushButton->isChecked() && ui->subgroup7MainPushButton->isChecked() && (!subgroupSoloActive || ui->subgroup7SoloPushButton->isChecked())) {
        subgroup7SampleBuffer.addTo(main1SampleBuffer);
    }
    if(!ui->subgroup8MutePushButton->isChecked() && ui->subgroup8MainPushButton->isChecked() && (!subgroupSoloActive || ui->subgroup8SoloPushButton->isChecked())) {
        subgroup8SampleBuffer.addTo(main2SampleBuffer);
    }

    // Peak detection    
    _subgroupPeak1 = QUnits::linearToDb(subgroup1SampleBuffer.peak());
    _subgroupPeak2 = QUnits::linearToDb(subgroup2SampleBuffer.peak());
    _subgroupPeak3 = QUnits::linearToDb(subgroup3SampleBuffer.peak());
    _subgroupPeak4 = QUnits::linearToDb(subgroup4SampleBuffer.peak());
    _subgroupPeak5 = QUnits::linearToDb(subgroup5SampleBuffer.peak());
    _subgroupPeak6 = QUnits::linearToDb(subgroup6SampleBuffer.peak());
    _subgroupPeak7 = QUnits::linearToDb(subgroup7SampleBuffer.peak());
    _subgroupPeak8 = QUnits::linearToDb(subgroup8SampleBuffer.peak());

    // Check if main is muted, and clear signal if necessary
    if(ui->main1MutePushButton->isChecked()) {
        main1SampleBuffer.clear();
    } else {
        _main1FaderStage->process(main1SampleBuffer);
    }

    if(ui->main2MutePushButton->isChecked()) {
        main2SampleBuffer.clear();
    } else {
        _main2FaderStage->process(main2SampleBuffer);
    }

    _mainPeak1 = QUnits::linearToDb(main1SampleBuffer.peak());
    _mainPeak2 = QUnits::linearToDb(main2SampleBuffer.peak());
}

QJsonObject MainMixerWidget::stateToJson()
{
    QJsonObject jsonObject;

    jsonObject.insert("subgroup1Gain", ui->subgroup1VolumeVerticalSlider->value());
    jsonObject.insert("subgroup2Gain", ui->subgroup2VolumeVerticalSlider->value());
    jsonObject.insert("subgroup3Gain", ui->subgroup3VolumeVerticalSlider->value());
    jsonObject.insert("subgroup4Gain", ui->subgroup4VolumeVerticalSlider->value());
    jsonObject.insert("subgroup5Gain", ui->subgroup5VolumeVerticalSlider->value());
    jsonObject.insert("subgroup6Gain", ui->subgroup6VolumeVerticalSlider->value());
    jsonObject.insert("subgroup7Gain", ui->subgroup7VolumeVerticalSlider->value());
    jsonObject.insert("subgroup8Gain", ui->subgroup8VolumeVerticalSlider->value());

    jsonObject.insert("main1Gain", ui->main1VolumeVerticalSlider->value());
    jsonObject.insert("main2Gain", ui->main2VolumeVerticalSlider->value());

    jsonObject.insert("subgroup1Muted", ui->subgroup1MutePushButton->isChecked());
    jsonObject.insert("subgroup2Muted", ui->subgroup2MutePushButton->isChecked());
    jsonObject.insert("subgroup3Muted", ui->subgroup3MutePushButton->isChecked());
    jsonObject.insert("subgroup4Muted", ui->subgroup4MutePushButton->isChecked());
    jsonObject.insert("subgroup5Muted", ui->subgroup5MutePushButton->isChecked());
    jsonObject.insert("subgroup6Muted", ui->subgroup6MutePushButton->isChecked());
    jsonObject.insert("subgroup7Muted", ui->subgroup7MutePushButton->isChecked());
    jsonObject.insert("subgroup8Muted", ui->subgroup8MutePushButton->isChecked());

    jsonObject.insert("main1Muted", ui->main1MutePushButton->isChecked());
    jsonObject.insert("main2Muted", ui->main2MutePushButton->isChecked());

    jsonObject.insert("subgroup1Soloed", ui->subgroup1SoloPushButton->isChecked());
    jsonObject.insert("subgroup2Soloed", ui->subgroup2SoloPushButton->isChecked());
    jsonObject.insert("subgroup3Soloed", ui->subgroup3SoloPushButton->isChecked());
    jsonObject.insert("subgroup4Soloed", ui->subgroup4SoloPushButton->isChecked());
    jsonObject.insert("subgroup5Soloed", ui->subgroup5SoloPushButton->isChecked());
    jsonObject.insert("subgroup6Soloed", ui->subgroup6SoloPushButton->isChecked());
    jsonObject.insert("subgroup7Soloed", ui->subgroup7SoloPushButton->isChecked());
    jsonObject.insert("subgroup8Soloed", ui->subgroup8SoloPushButton->isChecked());

    jsonObject.insert("subgroup1OnMain", ui->subgroup1MainPushButton->isChecked());
    jsonObject.insert("subgroup2OnMain", ui->subgroup2MainPushButton->isChecked());
    jsonObject.insert("subgroup3OnMain", ui->subgroup3MainPushButton->isChecked());
    jsonObject.insert("subgroup4OnMain", ui->subgroup4MainPushButton->isChecked());
    jsonObject.insert("subgroup5OnMain", ui->subgroup5MainPushButton->isChecked());
    jsonObject.insert("subgroup6OnMain", ui->subgroup6MainPushButton->isChecked());
    jsonObject.insert("subgroup7OnMain", ui->subgroup7MainPushButton->isChecked());
    jsonObject.insert("subgroup8OnMain", ui->subgroup8MainPushButton->isChecked());

    foreach(ChannelWidget *channelWidget, _registeredChannels) {
        int channelNumber = _registeredChannels.key(channelWidget);
        jsonObject.insert(QString("channel%1").arg(channelNumber), channelWidget->stateToJson());
    }

    return jsonObject;
}

void MainMixerWidget::stateFromJson(QJsonObject jsonObject)
{
    ui->subgroup1VolumeVerticalSlider->setValue(jsonObject.value("subgroup1Gain").toDouble());
    ui->subgroup2VolumeVerticalSlider->setValue(jsonObject.value("subgroup2Gain").toDouble());
    ui->subgroup3VolumeVerticalSlider->setValue(jsonObject.value("subgroup3Gain").toDouble());
    ui->subgroup4VolumeVerticalSlider->setValue(jsonObject.value("subgroup4Gain").toDouble());
    ui->subgroup5VolumeVerticalSlider->setValue(jsonObject.value("subgroup5Gain").toDouble());
    ui->subgroup6VolumeVerticalSlider->setValue(jsonObject.value("subgroup6Gain").toDouble());
    ui->subgroup7VolumeVerticalSlider->setValue(jsonObject.value("subgroup7Gain").toDouble());
    ui->subgroup8VolumeVerticalSlider->setValue(jsonObject.value("subgroup8Gain").toDouble());

    ui->main1VolumeVerticalSlider->setValue(jsonObject.value("main1Gain").toDouble());
    ui->main2VolumeVerticalSlider->setValue(jsonObject.value("main2Gain").toDouble());

    ui->subgroup1MutePushButton->setChecked(jsonObject.value("subgroup1Muted").toBool());
    ui->subgroup2MutePushButton->setChecked(jsonObject.value("subgroup2Muted").toBool());
    ui->subgroup3MutePushButton->setChecked(jsonObject.value("subgroup3Muted").toBool());
    ui->subgroup4MutePushButton->setChecked(jsonObject.value("subgroup4Muted").toBool());
    ui->subgroup5MutePushButton->setChecked(jsonObject.value("subgroup5Muted").toBool());
    ui->subgroup6MutePushButton->setChecked(jsonObject.value("subgroup6Muted").toBool());
    ui->subgroup7MutePushButton->setChecked(jsonObject.value("subgroup7Muted").toBool());
    ui->subgroup8MutePushButton->setChecked(jsonObject.value("subgroup8Muted").toBool());

    ui->main1MutePushButton->setChecked(jsonObject.value("main1Muted").toBool());
    ui->main2MutePushButton->setChecked(jsonObject.value("main2Muted").toBool());

    ui->subgroup1SoloPushButton->setChecked(jsonObject.value("subgroup1Soloed").toBool());
    ui->subgroup2SoloPushButton->setChecked(jsonObject.value("subgroup2Soloed").toBool());
    ui->subgroup3SoloPushButton->setChecked(jsonObject.value("subgroup3Soloed").toBool());
    ui->subgroup4SoloPushButton->setChecked(jsonObject.value("subgroup4Soloed").toBool());
    ui->subgroup5SoloPushButton->setChecked(jsonObject.value("subgroup5Soloed").toBool());
    ui->subgroup6SoloPushButton->setChecked(jsonObject.value("subgroup6Soloed").toBool());
    ui->subgroup7SoloPushButton->setChecked(jsonObject.value("subgroup7Soloed").toBool());
    ui->subgroup8SoloPushButton->setChecked(jsonObject.value("subgroup8Soloed").toBool());

    ui->subgroup1MainPushButton->setChecked(jsonObject.value("subgroup1OnMain").toBool());
    ui->subgroup2MainPushButton->setChecked(jsonObject.value("subgroup2OnMain").toBool());
    ui->subgroup3MainPushButton->setChecked(jsonObject.value("subgroup3OnMain").toBool());
    ui->subgroup4MainPushButton->setChecked(jsonObject.value("subgroup4OnMain").toBool());
    ui->subgroup5MainPushButton->setChecked(jsonObject.value("subgroup5OnMain").toBool());
    ui->subgroup6MainPushButton->setChecked(jsonObject.value("subgroup6OnMain").toBool());
    ui->subgroup7MainPushButton->setChecked(jsonObject.value("subgroup7OnMain").toBool());
    ui->subgroup8MainPushButton->setChecked(jsonObject.value("subgroup8OnMain").toBool());

    foreach(ChannelWidget *channelWidget, _registeredChannels) {
        int channelNumber = _registeredChannels.key(channelWidget);
        channelWidget->stateFromJson(jsonObject.value(QString("channel%1").arg(channelNumber)).toObject());
    }
}

void MainMixerWidget::updateInterface()
{
    QJackClient *jackClient = QJackClient::instance();
    QString displayText;
    displayText += QString("<table width=\"100%\"><tr><td><b>JACK Client</b></td><td></td></tr>");
    displayText += QString("<tr><td>RT processing:</td><td>%1</td></tr>").arg(jackClient->isRealtime() ? "Yes" : "No");
    displayText += QString("<tr><td>Buffers.:</td><td>%1 Samples</td></tr>").arg(jackClient->bufferSize());
    displayText += QString("<tr><td>CPU load:</td><td>%1</td></tr>").arg(jackClient->cpuLoad() < 1.0 ? "Idle" : QString("%1 %").arg((int)jackClient->cpuLoad()));
    displayText += QString("<tr><td>Samplerate:</td><td>%1 Hz</td></tr></table>").arg(jackClient->sampleRate());
    ui->displayLabel->setText(displayText);

    foreach(ChannelWidget *channelWidget, _registeredChannels) {
        channelWidget->updateInterface();
    }

    ui->subgroup1ProgressBar->setValue((int)_subgroupPeak1);
    ui->subgroup2ProgressBar->setValue((int)_subgroupPeak2);
    ui->subgroup3ProgressBar->setValue((int)_subgroupPeak3);
    ui->subgroup4ProgressBar->setValue((int)_subgroupPeak4);
    ui->subgroup5ProgressBar->setValue((int)_subgroupPeak5);
    ui->subgroup6ProgressBar->setValue((int)_subgroupPeak6);
    ui->subgroup7ProgressBar->setValue((int)_subgroupPeak7);
    ui->subgroup8ProgressBar->setValue((int)_subgroupPeak8);

    ui->main1ProgressBar->setValue((int)_mainPeak1);
    ui->main2ProgressBar->setValue((int)_mainPeak2);
}

void MainMixerWidget::on_clearPushButton_clicked()
{
    if(QMessageBox::Yes == QMessageBox::warning(this,
        tr("Reset controls"),
        tr("Are you sure you want to reset all controls?"),
        QMessageBox::Cancel, QMessageBox::Yes)) {
        resetControls();
    }
}

void MainMixerWidget::on_saveStatePushButton_clicked()
{
    QStringList homeLocations = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString targetFileName = QFileDialog::getSaveFileName(this,
                                                      tr("Save state"),
                                                      homeLocations.at(0),
                                                      tr("MX2482 State (*.mx2482)"));
    if(!targetFileName.isEmpty()) {
        if(!targetFileName.endsWith(".mx2482")) {
            targetFileName.append(".mx2482");
        }

        QFile file(targetFileName);

        if(file.open(QIODevice::WriteOnly)) {
            QJsonDocument jsonDocument(stateToJson());
            file.write(jsonDocument.toJson());
            file.close();
        } else {
            QMessageBox::critical(this,
                                  tr("Could not save state"),
                                  QString(tr("Could not open file for write: %1")).arg(targetFileName));
        }
    }
}

void MainMixerWidget::on_loadStatePushButton_clicked()
{
    QStringList homeLocations = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString targetFileName = QFileDialog::getOpenFileName(this,
                                                      tr("Load state"),
                                                      homeLocations.at(0),
                                                      tr("MX2482 State (*.mx2482)"));
    if(!targetFileName.isEmpty()) {
        QFile file(targetFileName);

        if(file.open(QIODevice::ReadOnly)) {
            QJsonDocument jsonDocument = QJsonDocument::fromJson(file.readAll());
            file.close();
            stateFromJson(jsonDocument.object());
        } else {
            QMessageBox::critical(this,
                                  tr("Could not load state"),
                                  QString(tr("Could not open file for read: %1")).arg(targetFileName));
        }
    }
}

void MainMixerWidget::on_aboutPushButton_clicked()
{
    AboutDialog aboutDialog;
    aboutDialog.exec();
}

void MainMixerWidget::resetControls()
{
    ui->subgroup1VolumeVerticalSlider->setValue(0);
    ui->subgroup2VolumeVerticalSlider->setValue(0);
    ui->subgroup3VolumeVerticalSlider->setValue(0);
    ui->subgroup4VolumeVerticalSlider->setValue(0);
    ui->subgroup5VolumeVerticalSlider->setValue(0);
    ui->subgroup6VolumeVerticalSlider->setValue(0);
    ui->subgroup7VolumeVerticalSlider->setValue(0);
    ui->subgroup8VolumeVerticalSlider->setValue(0);

    ui->main1VolumeVerticalSlider->setValue(0);
    ui->main2VolumeVerticalSlider->setValue(0);

    ui->subgroup1MutePushButton->setChecked(false);
    ui->subgroup2MutePushButton->setChecked(false);
    ui->subgroup3MutePushButton->setChecked(false);
    ui->subgroup4MutePushButton->setChecked(false);
    ui->subgroup5MutePushButton->setChecked(false);
    ui->subgroup6MutePushButton->setChecked(false);
    ui->subgroup7MutePushButton->setChecked(false);
    ui->subgroup8MutePushButton->setChecked(false);

    ui->main1MutePushButton->setChecked(false);
    ui->main2MutePushButton->setChecked(false);

    ui->subgroup1SoloPushButton->setChecked(false);
    ui->subgroup2SoloPushButton->setChecked(false);
    ui->subgroup3SoloPushButton->setChecked(false);
    ui->subgroup4SoloPushButton->setChecked(false);
    ui->subgroup5SoloPushButton->setChecked(false);
    ui->subgroup6SoloPushButton->setChecked(false);
    ui->subgroup7SoloPushButton->setChecked(false);
    ui->subgroup8SoloPushButton->setChecked(false);

    ui->subgroup1MainPushButton->setChecked(true);
    ui->subgroup2MainPushButton->setChecked(true);
    ui->subgroup3MainPushButton->setChecked(true);
    ui->subgroup4MainPushButton->setChecked(true);
    ui->subgroup5MainPushButton->setChecked(true);
    ui->subgroup6MainPushButton->setChecked(true);
    ui->subgroup7MainPushButton->setChecked(true);
    ui->subgroup8MainPushButton->setChecked(true);

    foreach(ChannelWidget *channelWidget, _registeredChannels) {
        channelWidget->resetControls();
    }
}
