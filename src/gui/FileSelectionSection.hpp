#ifndef FILE_SELECTION_SECTION_HPP
#define FILE_SELECTION_SECTION_HPP

#include <QDir>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

#include "ClickableLabel.hpp"
#include "CustomButton.hpp"

class FileSelectionSection : public QWidget
{
    Q_OBJECT

  public:
    explicit FileSelectionSection(QWidget* parent = nullptr);

    void updateFileInfo(const QString& fileName, const QString& filePath,
                        qint64 fileSize);

    QPushButton* getSendButton() const;

  signals:
    void fileSelected();
    void sendClicked();

  private:
    void setupUi();
    void setupConnections();

    QString formatFileSize(qint64 bytes);

    ClickableLabel* m_fileSelectionLabel;
    CustomButton*   m_sendFileButton;
};

#endif // FILE_SELECTION_SECTION_HPP
