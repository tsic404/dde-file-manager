// SPDX-FileCopyrightText: 2022 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "decoratorfileenumerator.h"

#include <QRegularExpression>

namespace dfmbase {

class DecoratorFileEnumeratorPrivate
{
public:
    explicit DecoratorFileEnumeratorPrivate(DecoratorFileEnumerator *q)
        : q(q)
    {
    }
    ~DecoratorFileEnumeratorPrivate() = default;

public:
    DecoratorFileEnumerator *q = nullptr;
    QSharedPointer<DFMIO::DEnumerator> denumerator = nullptr;
    QUrl url;
};

}

using namespace dfmbase;

static constexpr char kNetworkFilesRex[] { R"((^/run/user/.*/gvfs/|^/root/.gvfs/)(sftp|ftp|smb|dav))" };

DecoratorFileEnumerator::DecoratorFileEnumerator(const QString &filePath,
                                                 const QStringList &nameFilters /*= QStringList()*/,
                                                 DFMIO::DEnumerator::DirFilters filters /*= DFMIO::DEnumerator::DirFilter::NoFilter*/,
                                                 DFMIO::DEnumerator::IteratorFlags flags /*= DFMIO::DEnumerator::IteratorFlag::NoIteratorFlags*/)
    : d(new DecoratorFileEnumeratorPrivate(this))
{
    d->url = QUrl::fromLocalFile(filePath);
    d->denumerator.reset(new DFMIO::DEnumerator(d->url, nameFilters, filters, flags));
    if (filePath.contains(QRegularExpression(kNetworkFilesRex)))
        d->denumerator->setTimeout(2000);
}

DecoratorFileEnumerator::DecoratorFileEnumerator(const QUrl &url,
                                                 const QStringList &nameFilters /*= QStringList()*/,
                                                 DFMIO::DEnumerator::DirFilters filters /*= DFMIO::DEnumerator::DirFilter::NoFilter*/,
                                                 DFMIO::DEnumerator::IteratorFlags flags /*= DFMIO::DEnumerator::IteratorFlag::NoIteratorFlags*/)
    : d(new DecoratorFileEnumeratorPrivate(this))
{
    d->url = url;
    d->denumerator.reset(new DFMIO::DEnumerator(d->url, nameFilters, filters, flags));
    if (url.path().contains(QRegularExpression(kNetworkFilesRex)))
        d->denumerator->setTimeout(2000);
}

DecoratorFileEnumerator::DecoratorFileEnumerator(QSharedPointer<dfmio::DEnumerator> dfileEnumerator)
    : d(new DecoratorFileEnumeratorPrivate(this))
{
    d->url = dfileEnumerator->uri();
    d->denumerator = dfileEnumerator;
}

QSharedPointer<dfmio::DEnumerator> DecoratorFileEnumerator::enumeratorPtr()
{
    return d->denumerator;
}

bool DecoratorFileEnumerator::isValid() const
{
    return d->denumerator;
}

bool DecoratorFileEnumerator::hasNext() const
{
    if (d->denumerator)
        return d->denumerator->hasNext();

    return false;
}

QUrl DecoratorFileEnumerator::next() const
{
    if (d->denumerator)
        return d->denumerator->next();

    return QUrl();
}

QUrl DecoratorFileEnumerator::nextUrl() const
{
    return next();
}

quint64 DecoratorFileEnumerator::fileCount() const
{
    if (d->denumerator)
        return d->denumerator->fileCount();
    return 0;
}

QSharedPointer<dfmio::DFileInfo> DecoratorFileEnumerator::fileInfo() const
{
    if (d->denumerator)
        return d->denumerator->fileInfo();

    return nullptr;
}

DFMIOError DecoratorFileEnumerator::lastError() const
{
    if (d->denumerator)
        return d->denumerator->lastError();
    return DFMIOError();
}
