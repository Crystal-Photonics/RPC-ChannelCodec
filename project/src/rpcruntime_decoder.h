#ifndef RPCRUNTIMEDECODER_H
#define RPCRUNTIMEDECODER_H
#include "rpcruntime_decoder.h"
#include "rpcruntime_function.h"
#include "rpcruntime_protocol_description.h"
#include <QList>
#include <QTreeWidgetItem>
#include <functional>

typedef std::function<void(QString, QString, QPair<int,int>, QDateTime, int64_t)> watchCallBack_t;

class RPCWatchPoint{
public:
    RPCWatchPoint();
    RPCWatchPoint(QString FieldID,QString humanReadableName,QPair<int,int> plotIndex, watchCallBack_t callback);
    ~RPCWatchPoint();

    QString FieldID;
    QString humanReadableName;
    QPair<int,int> plotIndex;
    watchCallBack_t callback;

    void call(QDateTime timeStamp, int64_t val);


private:
    bool valid;
};

class RPCChannelCodec{
    public:
    RPCChannelCodec();
    ~RPCChannelCodec();
};

class RPCRuntimeDecodedParam{
public:
    RPCRuntimeDecodedParam(RPCRuntimeParamterDescription paramDescription);
    RPCRuntimeDecodedParam();
    ~RPCRuntimeDecodedParam();
    QByteArray decode(QByteArray inBuffer);
    QList<RPCRuntimeDecodedParam> subParams;
    RPCRuntimeParamterDescription getParamDescription() const;

    int64_t value;
    QString string;
    QString FieldID;
    bool isNull();


private:
    bool null;
    RPCRuntimeParamterDescription paramDescription;
};

class RPCRuntimeDecoder
{
public:
    RPCRuntimeDecoder(RPCRunTimeProtocolDescription protocolDescription);
    RPCRuntimeDecoder();
    ~RPCRuntimeDecoder();
    bool isNull();
    bool isReply();
    void setIsReply(bool reply);
    int getTransferLength(uint8_t ID);
    RPCRunTimeProtocolDescription protocolDescription;
    void setTimeStamp(QDateTime timeStamp);
    QByteArray RPCDecodeRPCData(QByteArray inBuffer);
    void RPCDecodeChannelCodedData(QByteArray inBuffer);
    QByteArray encodeToChannelCodedData(QByteArray inBuffer);

    QStringList getPrintableReport();
    QList<QTreeWidgetItem *> getTreeWidgetReport(QTreeWidgetItem *parent);

    void setChannelCodecOutput(QByteArray codecOutput);

    void clear();
    bool getErrorCRCHappened() const;
    void setErrorCRCHappened(bool value);

    bool getErrorChannelCodecHappened() const;
    void setErrorChannelCodecHappened(bool value);

    QString name;
    QString declaration;
    QList<RPCRuntimeDecodedParam> decodedParams;
    RPCRuntimeTransfer transfer;

    bool fieldExists(QString FieldID);
    RPCRuntimeDecodedParam getDecodedParamByFieldID(QString FieldID);

    RPCRuntimeTransfer getDecodedTransferByFieldID(QString FieldID);

    RPCRuntimeParamterDescription getParamDescriptionByFieldID(QString FieldID);

    void addWatchPoint(QString FieldID, QString humanReadableName, QPair<int,int> plotIndex, watchCallBack_t callback);
    void removeWatchPoint(QString FieldID);
    void clearWatchPoint();

    QList<RPCWatchPoint> getWatchPointList();

private:

    QByteArray decodeParams(QByteArray inBuffer, QString FieldID, QString OverwriteID, QList<RPCRuntimeParamterDescription> paramDescriptionList, QList<RPCRuntimeDecodedParam> &decodedParams);
    QStringList printsubType(int tabDepth, QList<RPCRuntimeDecodedParam> decodedParamList , bool isArrayField);
    QByteArray codecOutput;
    RPCRuntimeFunction getFunctionByID(uint8_t ID);
    bool isReplyByID(uint8_t ID, RPCRuntimeFunction fun);
    RPCRuntimeTransfer getTransfer(RPCRuntimeFunction fun, bool isReply);

    bool reply;
    bool errorCRCHappened;
    bool errorChannelCodecHappened;
    QDateTime timeStamp;
    QList<QTreeWidgetItem *> getTreeWidgetReport_recursive(QTreeWidgetItem *parent, QList<RPCRuntimeDecodedParam> decodedParamList, bool isArrayField);

    QList<RPCWatchPoint> watchpointList;

};

#endif // RPCRUNTIMEDECODERESULT_H
