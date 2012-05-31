#ifndef SIMPLESEARCHER_H
#define SIMPLESEARCHER_H

#include <QWidget>

#include <QVariantMap>
#include <QSslError>
#include <QUrl>

namespace Ui {
class SimpleSearcher;
}

class SimpleSearcher : public QWidget
{
    Q_OBJECT
    
public:
    explicit SimpleSearcher(QWidget *parent = 0);
    ~SimpleSearcher();

    void showTypeWidgets(bool);

    virtual void readSettings(const QString & companyName, const QString & appName);
    virtual void writeSettings(const QString & companyName, const QString & appName);

signals:
    void currentSchemaTypeChanged(const QString & id);

protected slots:
    virtual void onLinkClicked(const QUrl &);
    void setAwaitingMode();
    void dropAwaitingMode();
    void search();

private slots:
    void onJsonReady(int);

    void addSchemeType();
    void previousPage();
    void nextPage();

    void sslErrorHandler(class QNetworkReply * qnr, const QList<QSslError> & /*errlist*/);

    void showPosition(int pos);
    void onCurrentTypeChanged(int);

protected:
    virtual QString getCurrentType() const;
    virtual QString createHtml(const QVariantMap & map);
    virtual void getInfo(const QString & id, const QString &type);
    virtual bool delegatedRequest(const QVariantMap & ) { return false; }

    class QWebView * webView() const;
    void delegateRequests() { m_delegateMQLrequest = true; }
    void appendToHistory(const QString & value, int pos = -1);
    int  addItemToResultsList(const QString & item, const QVariant & itemData = QVariant());
    void addSchemeType(const QString & item, const QVariant & itemData = QVariant(), bool blockSignals = true );
    void setSearchText(const QString & text);

private:
    Ui::SimpleSearcher * ui;

    int m_historyPos;
    bool m_delegateMQLrequest;

    // first: mid, second: result position in comboBox
    // second = -1 -> undefined position

    typedef QPair<QString, int> HistoryNode;
//    struct HistoryNode {
//        QString searchText;
//        int limitPos;
//        QString typeId;
//        QString typeName;
//        int resultPos;
//    };
    QList<HistoryNode> m_history;

protected:
    class freebase_data_manager * m_pManager;
};

#endif // SIMPLESEARCHER_H
