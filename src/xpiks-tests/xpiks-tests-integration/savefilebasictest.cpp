#include "savefilebasictest.h"
#include <QUrl>
#include <QFileInfo>
#include <QStringList>
#include "integrationtestbase.h"
#include "signalwaiter.h"
#include "../../xpiks-qt/Commands/commandmanager.h"
#include "../../xpiks-qt/Models/artitemsmodel.h"
#include "../../xpiks-qt/MetadataIO/metadataiocoordinator.h"
#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/settingsmodel.h"
#include "../../xpiks-qt/Models/filteredartitemsproxymodel.h"
#include "../../xpiks-qt/Models/imageartwork.h"

QString SaveFileBasicTest::testName() {
    return QLatin1String("SaveFileBasicTest");
}

void SaveFileBasicTest::setup() {
}

int SaveFileBasicTest::doTest() {
    Models::ArtItemsModel *artItemsModel = m_CommandManager->getArtItemsModel();
    QList<QUrl> files;
    files << getFilePathForTest("images-for-tests/pixmap/seagull.jpg");

    MetadataIO::MetadataIOCoordinator *ioCoordinator = m_CommandManager->getMetadataIOCoordinator();
    SignalWaiter waiter;
    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));    

    int addedCount = artItemsModel->addLocalArtworks(files);
    VERIFY(addedCount == files.length(), "Failed to add file");
    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    Models::ArtworkMetadata *metadata = artItemsModel->getArtwork(0);
    Models::ImageArtwork *image = dynamic_cast<Models::ImageArtwork*>(metadata);

    VERIFY(image->getImageSize().width() == 1920, "Image width was read incorrectly");
    VERIFY(image->getImageSize().height() == 1272, "Image height was read incorrectly");

    QStringList keywords; keywords << "picture" << "seagull" << "bird";
    metadata->setDescription("Brand new description");
    metadata->setTitle("Brand new title");
    metadata->getBasicModel()->setKeywords(keywords);
    metadata->setIsSelected(true);

    bool doOverwrite = true, dontSaveBackups = false;

    QObject::connect(ioCoordinator, SIGNAL(metadataWritingFinished()), &waiter, SIGNAL(finished()));
    artItemsModel->saveSelectedArtworks(QVector<int>() << 0, doOverwrite, dontSaveBackups);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for writing metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while writing");

    artItemsModel->removeSelectedArtworks(QVector<int>() << 0);

    addedCount = artItemsModel->addLocalArtworks(files);
    VERIFY(addedCount == 1, "Failed to add file");

    QObject::connect(ioCoordinator, SIGNAL(metadataReadingFinished()), &waiter, SIGNAL(finished()));
    ioCoordinator->continueReading(true);

    if (!waiter.wait(20)) {
        VERIFY(false, "Timeout exceeded for reading metadata.");
    }

    VERIFY(!ioCoordinator->getHasErrors(), "Errors in IO Coordinator while reading");

    metadata = artItemsModel->getArtwork(0);
    const QStringList &actualKeywords = metadata->getKeywords();

    VERIFY(actualKeywords == keywords, "Read keywords are not the same");

    return 0;
}

