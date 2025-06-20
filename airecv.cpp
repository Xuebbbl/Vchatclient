#include "airecv.h"
#include <QNetworkRequest>
#include <QJsonArray>
#include <QDebug>

ApiHandler::ApiHandler(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void ApiHandler::startProcess(aiModel aimodel)
{
    if(aimodel==aiModel::commonAi){
       getAccessToken();
        return;

    }else if(aimodel==aiModel::deepseek_r1){
         deepseek_r1("deepseek-r1");


    }else if(aimodel==aiModel::deepseek_v3){

          deepseek_r1("deepseek-v3");
    }
    // if(aimodel==aiModel::commonAi){
    //    getAccessToken();
    //     return;

    // }else if(aimodel==aiModel::deepseek_r1){
    //      deepseek_r1("deepseek-reasoner");


    // }else if(aimodel==aiModel::deepseek_v3){

    //           deepseek_r1("deepseek-chat");
    //     }
}

void ApiHandler::setProblem(QString str)
{
    problem=str;
}

void ApiHandler::getAccessToken()
{
    const QUrl tokenUrl("https://aip.baidubce.com/oauth/2.0/token?"
                        "grant_type=client_credentials&"
                        "client_id=9UwYgcx1xwb4TbVoXvoL7wIJ&"
                        "client_secret=cetAzx8B23j3eAVhve08nrXKFo0iefLQ");

    QNetworkRequest request(tokenUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager->post(request, QByteArray());
    connect(reply, &QNetworkReply::finished, [=]() {
        handleTokenResponse(reply);
        reply->deleteLater();
    });
}

void ApiHandler::handleTokenResponse(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Token Error:" << reply->errorString();
        return;
    }


    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject json = doc.object();

    if (json.contains("access_token")) {
        accessToken = json["access_token"].toString();
        qDebug()<<accessToken;
        qDebug() << "Access Token obtained successfully";
        sendChatRequest();
    } else {
        qDebug() << "Failed to get access token:" << doc.toJson();
    }
}

void ApiHandler::sendChatRequest()
{
    const QUrl chatUrl(QString("https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/yi_34b_chat?access_token=%1")
                           .arg(accessToken));

    QNetworkRequest request(chatUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject message;
    message["role"] = "user";
    message["content"] =problem;

    QJsonObject requestBody;
    requestBody["messages"] = QJsonArray() << message;

    QNetworkReply *reply = manager->post(request, QJsonDocument(requestBody).toJson());
    connect(reply, &QNetworkReply::finished, [=]() {
        handleChatResponse(reply);
        reply->deleteLater();
    });
}

void ApiHandler::handleChatResponse(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Chat Error:" << reply->errorString();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject json = doc.object();

    if (json.contains("result")) {
        QString result = json["result"].toString();
        emit reciveString(json["result"].toString());
        qDebug() << "Received Response:" << result;
    } else {
        qDebug() << "Invalid response format:" << doc.toJson();
        emit reciveString("noAnswer");
    }


}

void ApiHandler::deepseek_r1(QString model)
{
    // 设置请求URL
    QUrl url("https://qianfan.baidubce.com/v2/chat/completions");
    QNetworkRequest request(url);
    // 设置请求头
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(
        "Authorization",
        "Bearer bce-v3/ALTAK-JY3bs00ojFcp5PhoEHRGo/d7748e8b002e910e3b8876e760a859f6710339fd"
        );
    // 构建JSON请求体
    QJsonObject jsonBody;
    jsonBody["model"] = model;

    QJsonArray messagesArray;
    QJsonObject messageObject;
    messageObject["role"] = "user";
    messageObject["content"] = problem;
    messagesArray.append(messageObject);

    jsonBody["messages"] = messagesArray;
    // 将JSON转换为字节数组
    QJsonDocument jsonDoc(jsonBody);
    QByteArray postData = jsonDoc.toJson();

    // 发送POST请求
    QNetworkReply* reply = manager->post(request, postData);
    connect(reply, &QNetworkReply::finished, [=]() {
        // 处理响应
        QByteArray response;
        if (reply->error() == QNetworkReply::NoError) {
            response = reply->readAll();
              QString jsonStr=QString::fromUtf8(response);
            qDebug()<<jsonStr;
                // 将JSON字符串转换为QJsonDocument对象
            QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8());

            // 检查JSON文档是否有效
            if (jsonDoc.isNull()) {
                qDebug() << "无效的JSON数据";
                return "";
            }

            // 获取根对象
            QJsonObject rootObj = jsonDoc.object();

            // 检查是否包含 "choices" 数组
            if (!rootObj.contains("choices") || !rootObj["choices"].isArray()) {
                qDebug() << "未找到 'choices' 数组";
                return "";
            }

            // 获取 "choices" 数组
            QJsonArray choicesArray = rootObj["choices"].toArray();

            // 检查数组是否为空
            if (choicesArray.isEmpty()) {
                qDebug() << "'choices' 数组为空";
                return "";
            }

            // 获取第一个元素
            QJsonObject firstChoice = choicesArray.first().toObject();

            // 检查是否包含 "message" 对象
            if (!firstChoice.contains("message") || !firstChoice["message"].isObject()) {
                qDebug() << "未找到 'message' 对象";
                return "";
            }

            // 获取 "message" 对象
            QJsonObject messageObj = firstChoice["message"].toObject();

            // 检查是否包含 "content" 字段
            if (!messageObj.contains("content") || !messageObj["content"].isString()) {
                qDebug() << "未找到 'content' 字段";
                return "";
            }

            // 获取 "content" 字段的值
            emit reciveString( messageObj["content"].toString().remove(0,2));
            // emit reciveString(QString(response));
        } else {
            qDebug() << "请求失败:" << reply->errorString();
            response = QString("{\"error\":\"%1\"}").arg(reply->errorString()).toUtf8();
           emit reciveString("noAnswer");

        }
        // 清理资源
        reply->deleteLater();
    });
    // // 设置请求URL
    // QUrl url("https://api.deepseek.com/chat/completions");
    // QNetworkRequest request(url);
    // // 设置请求头
    // request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    // request.setRawHeader(
    //     "Authorization",
    //     "Bearer sk-dcc2da080d124a66b22cba48bb5c935c"
    //     );
    // // 构建JSON请求体
    // QJsonObject jsonBody;
    // jsonBody["model"] = model;

    // QJsonArray messagesArray;
    // QJsonObject messageObject;
    // messageObject["role"] = "user";
    // messageObject["content"] = problem;
    // messagesArray.append(messageObject);

    // jsonBody["messages"] = messagesArray;
    // jsonBody["stream"]="false";
    // // 将JSON转换为字节数组
    // QJsonDocument jsonDoc(jsonBody);
    // QByteArray postData = jsonDoc.toJson();

    // // 发送POST请求
    // QNetworkReply* reply = manager->post(request, postData);
    // connect(reply, &QNetworkReply::finished, [=]() {
    //     // 处理响应
    //     QByteArray response;
    //     if (reply->error() == QNetworkReply::NoError) {
    //         response = reply->readAll();
    //         emit reciveString(QString(response));
    //     } else {
    //         qDebug() << "请求失败:" << reply->errorString();
    //         response = QString("{\"error\":\"%1\"}").arg(reply->errorString()).toUtf8();
    //        emit reciveString("noAnswer");

    //     }
    //     // 清理资源
    //     reply->deleteLater();
    // });

}
