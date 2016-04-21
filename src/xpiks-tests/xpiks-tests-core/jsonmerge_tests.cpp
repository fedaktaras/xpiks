#include "jsonmerge_tests.h"
#include "../../xpiks-qt/Common/defines.h"
#include "../../xpiks-qt/Helpers/jsonhelper.h"
#include "../../xpiks-qt/Helpers/localconfig.h"

void JsonMergeTests::initTestCase() {
    LOG_DEBUG << "App path: " << QDir::currentPath();
}

void JsonMergeTests::mergeTwoFilesObjects() {
    QFileInfo infoOld("jsons-for-tests/old.json");
    QFileInfo infoNew("jsons-for-tests/new.json");
    QFileInfo infoGold("jsons-for-tests/gold.json");

    QString pathTo = infoOld.absoluteFilePath();
    QString pathWith = infoNew.absoluteFilePath();
    QString pathGold = infoGold.absoluteFilePath();

    QVERIFY(infoOld.exists());
    QVERIFY(infoNew.exists());
    QVERIFY(infoGold.exists());

    Helpers::LocalConfig localConfigWith;
    Helpers::LocalConfig localConfigTo;
    Helpers::LocalConfig localConfigGold;

    localConfigWith.initConfig(pathWith);
    localConfigTo.initConfig(pathTo);
    localConfigGold.initConfig(pathGold);

    Helpers::mergeJson(localConfigWith.getConfig(), localConfigTo.getConfig(), 0, *this);

    QVERIFY(localConfigGold.getConfig() == localConfigTo.getConfig());
}

void JsonMergeTests::mergeTwoFilesStrings() {
    QFileInfo infoOld("jsons-for-tests/oldS.json");
    QFileInfo infoNew("jsons-for-tests/newS.json");
    QFileInfo infoGold("jsons-for-tests/goldS.json");

    QString pathTo = infoOld.absoluteFilePath();
    QString pathWith = infoNew.absoluteFilePath();
    QString pathGold = infoGold.absoluteFilePath();

    QVERIFY(infoOld.exists());
    QVERIFY(infoNew.exists());
    QVERIFY(infoGold.exists());

    Helpers::LocalConfig localConfigWith;
    Helpers::LocalConfig localConfigTo;
    Helpers::LocalConfig localConfigGold;

    localConfigWith.initConfig(pathWith);
    localConfigTo.initConfig(pathTo);
    localConfigGold.initConfig(pathGold);

    Helpers::mergeJson(localConfigWith.getConfig(), localConfigTo.getConfig(), 0, *this);

    const QJsonDocument &goldConfig = localConfigGold.getConfig();
    const QJsonDocument &testConfig = localConfigTo.getConfig();
    QJsonObject goldObject = goldConfig.object();
    QJsonObject testObject = testConfig.object();
    QJsonArray goldArray = goldObject["ftp_list"].toArray();
    QJsonArray testArray = testObject["ftp_list"].toArray();
    QSet<QString> goldSet;
    QSet<QString> testSet;
    int goldSize = goldArray.size();
    int testSize = testArray.size();

    for (int i = 0; i < goldSize; i++ ){
        goldSet.insert(goldArray[i].toString());
    }

    for (int i = 0; i < testSize; i++ ){
        testSet.insert(testArray[i].toString());
    }

    QVERIFY(testSet == goldSet);
}
