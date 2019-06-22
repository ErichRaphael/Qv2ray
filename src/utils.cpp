#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

#include "utils.h"

QJsonObject switchJsonArrayObject(QJsonObject obj, QString value)
{
    QJsonObject returnObj = obj.value(value).isNull()
                ? obj.value(value).toObject()
                : obj.value(value).toArray().first().toObject();
    return returnObj;
}

QJsonObject findValueFromJsonArray(QJsonArray arr, QString key, QString val)
{
    for (const auto obj : arr) {
        if (obj.toObject().value(key).toString() == val) {
            return obj.toObject();
        }
    }
    return QJsonObject();
}

QJsonObject loadRootObjFromConf()
{
    QFile globalConfigFile("Hv2ray.conf");
    globalConfigFile.open(QIODevice::ReadOnly);
    QByteArray conf = globalConfigFile.readAll();
    globalConfigFile.close();
    QJsonDocument v2conf(QJsonDocument::fromJson(conf));
    QJsonObject rootObj = v2conf.object();
    return rootObj;
}

QJsonArray getInbounds()
{
    QJsonArray inbounds;
    inbounds = loadRootObjFromConf().value("inbounds").toArray();
    return inbounds;
}

bool getRootEnabled()
{
    return loadRootObjFromConf().value("v2suidEnabled").toBool();
}

bool checkVCoreExes()
{
    if (QFileInfo("v2ray").exists() && QFileInfo("geoip.dat").exists() && QFileInfo("geosite.dat").exists() && QFileInfo("v2ctl").exists()) {
        return true;
    } else {
        showWarnMessageBox(nullptr, "v2ray core not found", "V2ray core files not found. Please download the latest version of v2ray and extract it into the current folder.");
        return false;
    }
}

void showWarnMessageBox(QWidget* parent, QString title, QString text)
{
    QMessageBox::warning(parent, title, text, QMessageBox::Ok | QMessageBox::Default, 0);
}

void overrideInbounds(QString path)
{
    QFile confFile(path);
    confFile.open(QIODevice::ReadOnly);
    QByteArray conf = confFile.readAll();
    confFile.close();
    QJsonDocument v2conf(QJsonDocument::fromJson(conf));
    QJsonObject rootObj = v2conf.object();
    QJsonArray modifiedIn = getInbounds();
    rootObj.remove("inbounds");
    rootObj.remove("inbound");
    rootObj.remove("inboundDetour");
    rootObj.insert("inbounds", QJsonValue(modifiedIn));
    v2conf.setObject(rootObj);
    conf = v2conf.toJson(QJsonDocument::Indented);
    confFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    confFile.write(conf);
    confFile.close();
}

int getIndexByValue(QJsonArray array, QString key, QString val)
{
    QJsonArray::iterator it;
    int index = 0;
    for(it = array.begin(); it != array.end(); it ++) {
        if(it->toObject().value(key) == val) {
            return index;
        }
        index ++;
    }
    return -1;
}
