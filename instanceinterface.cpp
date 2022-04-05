#include "instanceinterface.h"
#include "globalfunction.h"
#include "moduleconfigform.h"
#include "scaraconfigform.h"
#include "axisconfigform.h"
#include "pumaconfigform.h"


InstanceInterface::InstanceInterface(QWidget *parent) : QWidget(parent)
{

}

BaseWidget *InstanceInterface::getInstance(QString path)
{
    QByteArray docData;
    QString rightPath = path;
    Json::Value root = GlobalFunction::ReadJsonFromJsonfile(rightPath);
#define INSTANCE_FACTORY(INSTANCE, OBJ)\
    if(QString::fromStdString(root["type"].asString()) == #INSTANCE)\
    {\
        return new OBJ(rightPath);\
    }
    INSTANCE_FACTORY(axis, AxisConfigForm)
    INSTANCE_FACTORY(scara, ScaraConfigForm)
    INSTANCE_FACTORY(puma, PumaConfigForm)
    INSTANCE_FACTORY(module, ModuleConfigForm)
    return nullptr;
}
