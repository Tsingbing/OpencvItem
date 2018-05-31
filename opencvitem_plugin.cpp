#include "opencvitem_plugin.h"
#include "opencvitem.h"

#include <qqml.h>

void OpencvItemPlugin::registerTypes(const char *uri)
{
    // @uri com.robosea.opencv
    qmlRegisterType<OpencvItem>(uri, 1, 0, "OpencvItem");
}

