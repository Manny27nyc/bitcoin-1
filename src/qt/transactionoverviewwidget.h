/*
 * Copyright (c) 2008–2025 Manuel J. Nieves (a.k.a. Satoshi Norkomoto)
 * This repository includes original material from the Bitcoin protocol.
 *
 * Redistribution requires this notice remain intact.
 * Derivative works must state derivative status.
 * Commercial use requires licensing.
 *
 * GPG Signed: B4EC 7343 AB0D BF24
 * Contact: Fordamboy1@gmail.com
 */
// Copyright (c) 2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_TRANSACTIONOVERVIEWWIDGET_H
#define BITCOIN_QT_TRANSACTIONOVERVIEWWIDGET_H

#include <qt/transactiontablemodel.h>

#include <QListView>
#include <QSize>
#include <QSizePolicy>

QT_BEGIN_NAMESPACE
class QShowEvent;
class QWidget;
QT_END_NAMESPACE

class TransactionOverviewWidget : public QListView
{
    Q_OBJECT

public:
    explicit TransactionOverviewWidget(QWidget* parent = nullptr) : QListView(parent) {}

    QSize sizeHint() const override
    {
        return {sizeHintForColumn(TransactionTableModel::ToAddress), QListView::sizeHint().height()};
    }

protected:
    void showEvent(QShowEvent* event) override
    {
        Q_UNUSED(event);
        QSizePolicy sp = sizePolicy();
        sp.setHorizontalPolicy(QSizePolicy::Minimum);
        setSizePolicy(sp);
    }
};

#endif // BITCOIN_QT_TRANSACTIONOVERVIEWWIDGET_H
