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

signals:
    void currentSchemaTypeChanged(const QString & id);

protected slots:
    virtual void onLinkClicked(const QUrl &);

private slots:
    void onJsonReady(int);

    void addSchemeType();
    void previousPage();
    void nextPage();

    void sslErrorHandler(class QNetworkReply * qnr, const QList<QSslError> & /*errlist*/);
    void search();

    void showPosition(int pos);
    void onCurrentTypeChanged(int);

protected:
    virtual QString getCurrentType() const;
    virtual QString createHtml(const QVariantMap & map);
    virtual void getInfo(const QString & id, const QString &type);
    virtual void delegatedRequest(const QVariantMap & ) {}

    void delegateRequests() { m_delegateMQLrequest = true; }
    class QWebView * webView() const;
    void appendToHistory(const QString & value, int pos = -1);
    int addItemToResultsList(const QString & item, const QVariant & itemData = QVariant());

private:
    Ui::SimpleSearcher * ui;

    int m_historyPos;
    bool m_delegateMQLrequest;

    // first: mid, second: result position in comboBox
    // second = -1 -> undefined position

    typedef QPair<QString, int> HistoryNode;
    QList<HistoryNode> m_history;

protected:
    class freebase_data_manager * m_pManager;
};

#endif // SIMPLESEARCHER_H
