#include "modelpage.h"

modelPage::modelPage(int index, QString label, QObject *parent) :
    internalIndex(index), label(label), QObject(parent)
{

}
