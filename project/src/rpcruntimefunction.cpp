#include <QDebug>
#include "rpcruntimefunction.h"



RPCRuntimeFunction::RPCRuntimeFunction()
{

}

RPCRuntimeFunction::~RPCRuntimeFunction()
{

}

RPCRuntimeParamter::RPCRuntimeParamter()
{
    elementBitLength = 0;
    elementCount = 1;
    indexPosition = 0;
    name = "";
    typeName = "";
    rpcParamType = RPCParamType_t::param_none ;
    isSigned = true;
}

RPCRuntimeParamter::~RPCRuntimeParamter()
{

}

RPCRuntimeTransfer::RPCRuntimeTransfer(){
    isNull = false;
}

RPCRuntimeTransfer::~RPCRuntimeTransfer(){

}

bool RPCRuntimeTransfer::loadParamListFromXML(QDomElement xmlParams)
{
    while (!xmlParams.isNull()){

        if (xmlParams.tagName()=="parameter"){
            RPCRuntimeParamter param;
            if (!param.loadFromXML(xmlParams)){
                return false;
            }
            paramList.append(param);
        }
        xmlParams = xmlParams.nextSibling().toElement();
    }
    return true;
}

bool RPCRuntimeParamter::loadFromXML(QDomElement xmlParams){
    bool ok = true;
    bool ok_gesamt = true;
    bool isSubType = false;

    if (xmlParams.tagName() == "array"){
        isSubType = true;
    }
    typeName = xmlParams.attribute("ctype","");
    elementBitLength = xmlParams.attribute("bits","").toInt(&ok);
    ok_gesamt &= ok;
    elementCount = xmlParams.attribute("elements","").toInt(&ok);
    ok_gesamt &= ok;
    if(isSubType == false){
        name = xmlParams.attribute("name","");
        indexPosition = xmlParams.attribute("position","").toInt(&ok);
        ok_gesamt |= ok;
    }

    if (!setTypeByString(xmlParams.attribute("type",""))){
        //  qCritical() << "unknown type in function: " << parent->name << "and parameter" << param.name ;
        return false;
    }

    if (rpcParamType == RPCParamType_t::param_array){
        QDomElement xmlSubParams=xmlParams.firstChild().toElement();
        while(!xmlSubParams.isNull()){
            RPCRuntimeParamter subparam;
            if (!subparam.loadFromXML(xmlSubParams)){
                return false;
            }

            subParamters.append(subparam);
            elementCount = subparam.elementCount;
            xmlSubParams = xmlSubParams.nextSibling().toElement();
        }

    }
    QDomElement paramSigned = xmlParams.firstChildElement("integer");

    if (!paramSigned.isNull()){
        isSigned = paramSigned.attribute("signed","").toLower() == QString("true");
    }else{
        if (rpcParamType == RPCParamType_t::param_int) {
           // qCritical() << "didnt found whether int is signed or not in function: " << parent->name << "and parameter" << param.name ;
            return false;
        }
    }

    QDomElement paramEnum = xmlParams.firstChildElement("enum");

    if (!paramEnum.isNull()){
        while(!paramEnum.isNull()){
            if (paramEnum.tagName()=="enum"){
                QPair<int,QString> enumItem;
                bool ok;
                enumItem.first = paramEnum.attribute("value","").toInt(&ok);
                enumItem.second = paramEnum.attribute("name","");
                if(enumItem.second == ""){
                  //  qCritical() << "enum name not valid in function: " << parent->name << "and parameter" << param.name ;
                    return false;
                }
                if(!ok){
                   // qCritical() << "enum value not valid in function: " << parent->name << "and parameter" << param.name ;
                    return false;
                }
                enumValues.append(enumItem);
            }
            paramEnum = paramEnum.nextSibling().toElement();
        }
    }else{
        if (rpcParamType == RPCParamType_t::param_enum) {
        //    qCritical() << "didnt found whether int is signed or not in function: " << parent->name << "and parameter" << param.name ;
            return false;
        }
    }

    if ((name=="")&&(isSubType==false) ){
       // qCritical() << "didnt found param name in function: " << parent->name;
        return false;
    }
    if ((typeName=="")&&(isSubType==false)){
       // qCritical() << "didnt found type name in function: " << parent->name;
        return false;
    }
    return true;
}

bool RPCRuntimeParamter::setTypeByString(QString typeName)
{
    if (typeName == "integer"){
        rpcParamType = RPCParamType_t::param_int;
        return true;
    }else if(typeName == "enum"){
        rpcParamType = RPCParamType_t::param_enum;
        return true;
    }else if(typeName == "array"){
        rpcParamType = RPCParamType_t::param_array;
        return true;
    }else if(typeName == "character"){
        rpcParamType = RPCParamType_t::param_character;
        return true;
    }else{
        return false;
    }

}



