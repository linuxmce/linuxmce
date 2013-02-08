

#ifndef IOSHELPERS_H
#define IOSHELPERS_H

#ifdef IOS
static QString qStringFromNSString(NSString *nsstring)
{
    return QString::fromUtf8([nsstring UTF8String]);
}

static QString documentsDirectory()
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    return qStringFromNSString(documentsDirectory);
}
#endif

#endif // IOSHELPERS_H
