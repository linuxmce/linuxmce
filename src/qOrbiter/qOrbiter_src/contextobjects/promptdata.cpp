#include "promptdata.h"

PromptData::PromptData(QString title, int data, QObject *parent) :
    QObject(parent), dataTitle(title), data_id(data)
{
}
