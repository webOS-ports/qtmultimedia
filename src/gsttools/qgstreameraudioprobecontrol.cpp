/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Toolkit.
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

#include "qgstreameraudioprobecontrol_p.h"
#include <private/qgstutils_p.h>

QGstreamerAudioProbeControl::QGstreamerAudioProbeControl(QObject *parent)
    : QMediaAudioProbeControl(parent)
{

}

QGstreamerAudioProbeControl::~QGstreamerAudioProbeControl()
{

}

#if GST_CHECK_VERSION(1,0,0)
void QGstreamerAudioProbeControl::bufferProbed(GstBuffer* buffer, GstCaps* caps)
{
#else
void QGstreamerAudioProbeControl::bufferProbed(GstBuffer* buffer)
{
    gst_buffer_get_caps(buffer);
#endif
    if (!caps)
        return;

    QAudioFormat format = QGstUtils::audioFormatForCaps(caps);
    gst_caps_unref(caps);
    if (!format.isValid())
        return;

    #if GST_CHECK_VERSION(1,0,0)

    GstMapInfo info;

    gst_buffer_map (buffer, &info, GST_MAP_READ);
    QAudioBuffer audioBuffer = QAudioBuffer(QByteArray((const char*)info.data, info.size), format);
    gst_buffer_unmap(buffer, &info);

    #else

    QAudioBuffer audioBuffer = QAudioBuffer(QByteArray((const char*)buffer->data, buffer->size), format);

    #endif

    {
        QMutexLocker locker(&m_bufferMutex);
        m_pendingBuffer = audioBuffer;
        QMetaObject::invokeMethod(this, "bufferProbed", Qt::QueuedConnection);
    }
}

void QGstreamerAudioProbeControl::bufferProbed()
{
    QAudioBuffer audioBuffer;
    {
        QMutexLocker locker(&m_bufferMutex);
        if (!m_pendingBuffer.isValid())
            return;
        audioBuffer = m_pendingBuffer;
    }
    emit audioBufferProbed(audioBuffer);
}
