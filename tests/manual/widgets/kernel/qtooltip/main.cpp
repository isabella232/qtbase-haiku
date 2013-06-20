/****************************************************************************
**
** Copyright (C) 2013 Thorbjørn Lund Martsum - tmartsum[at]gmail.com
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QTest>
#include <QDialog>
#include <QToolTip>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class TestDialog : public QDialog
{
    Q_OBJECT
public:
    TestDialog();
protected slots:
    void showSomeToolTips();
};

void TestDialog::showSomeToolTips()
{
    QPoint p(100 + 20, 100 + 20);

    for (int u = 1; u < 20; u += 5) {
        QString s = tr("Seconds: ") + QString::number(u);
        QToolTip::showText(p, s, 0, QRect(), 1000 * u);
        QTest::qWait((u + 1) * 1000);
    }

    QToolTip::showText(p, tr("Seconds: 2"), 0, QRect(), 2000);
    QTest::qWait(3000);

    QToolTip::showText(p, tr("Standard label"), 0, QRect());
    QTest::qWait(12000);
}

TestDialog::TestDialog()
{
    // Notice that these tool tips will disappear if another tool tip is shown.
    QLabel *label1 = new QLabel(tr("Tooltip - Only two seconds display"));
    label1->setToolTip(tr("2 seconds display"));
    label1->setToolTipDuration(2000);
    Q_ASSERT(label1->toolTipDuration() == 2000);

    QLabel *label2 = new QLabel(tr("Tooltip - 30 seconds display time"));
    label2->setToolTip(tr("30 seconds display"));
    label2->setToolTipDuration(30000);

    QPushButton *pb = new QPushButton(tr("&Test"));
    pb->setToolTip(tr("Show some tool tips."));
    Q_ASSERT(pb->toolTipDuration() == -1);
    connect(pb, SIGNAL(clicked()), this, SLOT(showSomeToolTips()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label1);
    layout->addWidget(label2);
    layout->addWidget(pb);

    setLayout(layout);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestDialog dlg;
    dlg.show();
    return app.exec();
}

#include "main.moc"