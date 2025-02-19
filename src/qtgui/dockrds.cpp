/* -*- c++ -*- */
/*
 * Gqrx SDR: Software defined radio receiver powered by GNU Radio and Qt
 *           https://gqrx.dk/
 *
 * Copyright 2011-2013 Alexandru Csete OZ9AEC.
 *
 * Gqrx is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Gqrx is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gqrx; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#include <QDebug>
#include <QVariant>
#include "dockrds.h"
#include "ui_dockrds.h"

DockRDS::DockRDS(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockRDS)
{
    ui->setupUi(this);

    ui->rdsText->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
}

DockRDS::~DockRDS()
{
    delete ui;
}

void DockRDS::updateRDS(QString text, int type)
{
    std::string     str, out;

    /* type 0 = PI
     * type 1 = PS
     * type 2 = PTY
     * type 3 = flagstring: TP, TA, MuSp, MoSt, AH, CMP, stPTY
     * type 4 = RadioText
     * type 5 = ClockTime
     * type 6 = Alternative Frequencies
     */
    switch (type)
    {
    case 0:
        emit rdsPI(text);
        ui->program_information->setText(text);
        break;
    case 1:
        ui->station_name->setText(text);
        break;
    case 2:
        ui->program_type->setText(text);
        break;
    case 3:
        str = text.toStdString();
        out = "";
        if (str.at(0) == '1') out.append("TP ");
        if (str.at(1) == '1') out.append("TA ");
        if (str.at(2) == '0') out.append("Speech ");
        if (str.at(2) == '1') out.append("Music ");
        if (str.at(3) == '0') out.append("Stereo ");
        if (str.at(3) == '1') out.append("Mono ");
        if (str.at(4) == '1') out.append("AH ");
        if (str.at(5) == '1') out.append("CMP ");
        if (str.at(6) == '1') out.append("stPTY ");
        ui->flags->setText(QString::fromStdString(out));
        break;
    case 4:
        ui->radiotext->setText(text);
        break;
    case 5:
        ui->clocktime->setText(text);
        break;
    case 6:
        ui->alt_freq->setText(text);
        break;
    default:
        // nothing to do
        break;
    }
}

void DockRDS::updateSIS(std::string text[6])
{
    ui->nrsc5_station_name->setText(QString::fromStdString(text[0]));
    ui->nrsc5_slogan->setText(QString::fromStdString(text[1]));
    ui->nrsc5_message->setText(QString::fromStdString(text[2]));
    ui->nrsc5_alert->setText(QString::fromStdString(text[3]));
    ui->nrsc5_country_code->setText(QString::fromStdString(text[4]));
    ui->nrsc5_facility_id->setText(QString::fromStdString(text[5]));
}
void DockRDS::updateID3(std::string text[4])
{
    ui->nrsc5_title->setText(QString::fromStdString(text[0]));
    ui->nrsc5_artist->setText(QString::fromStdString(text[1]));
    ui->nrsc5_album->setText(QString::fromStdString(text[2]));
    ui->nrsc5_genre->setText(QString::fromStdString(text[3]));
}

void DockRDS::ClearTextFields()
{
    ui->program_information->setText("");
    ui->station_name->setText("");
    ui->program_type->setText("");
    ui->flags->setText("");
    ui->radiotext->setText("");
    ui->clocktime->setText("");
    ui->alt_freq->setText("");

    /* NRSC-5 SIS */
    ui->nrsc5_alert->setText("");
    ui->nrsc5_country_code->setText("");
    ui->nrsc5_facility_id->setText("");
    ui->nrsc5_message->setText("");
    ui->nrsc5_slogan->setText("");
    ui->nrsc5_station_name->setText("");
}

void DockRDS::showEnabled()
{
    ClearTextFields();
    if (!ui->rdsCheckbox->isChecked())
    {
        ui->rdsCheckbox->blockSignals(true);
        ui->rdsCheckbox->setChecked(true);
        ui->rdsCheckbox->blockSignals(false);
    }
}

void DockRDS::showDisabled()
{
    ClearTextFields();
}

/** Enable/disable RDS decoder */
void DockRDS::on_rdsCheckbox_toggled(bool checked)
{
    emit rdsDecoderToggled(checked);
}

/* used by remote control */
void DockRDS::setRDSmode(bool cmd)
{
    if (!ui->rdsCheckbox->isEnabled())
        return;

    ui->rdsCheckbox->setChecked(cmd);
}

void DockRDS::setCurrentPage(int index)
{
    if (index < PAGE_NUM)
    {
        ui->stackedWidget->setCurrentIndex(index);

        // the checkbox can cause a crash if it is out of sync
        if (index != PAGE_RDS)
        {
            ui->rdsCheckbox->blockSignals(true);
            ui->rdsCheckbox->setChecked(false);
            ui->rdsCheckbox->blockSignals(false);
        }
    }
}
