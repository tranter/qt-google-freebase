#ifndef SIMPLESEARCHERHISTORY_H
#define SIMPLESEARCHERHISTORY_H

#include <QObject>
#include <QList>
#include <QSharedPointer>
#include <QPair>

class SimpleSearcher;

class SimpleSearcherHistory : public QObject
{
    Q_OBJECT
public:
    SimpleSearcherHistory(SimpleSearcher * s);

    bool hasPrevious() const { return 0 < m_historyPos; }
    bool hasNext() const { return m_historyPos+1 < m_history.count(); }

public slots:
    bool loadPreviousData();
    bool loadNextData();

    void saveData();

private:
    void remove(int from);
    void restore(int pos);

    struct HistoryNode
    {
        // in node we will be store all informatoion from GUI like:
        // results list, types list, and other...

        typedef QPair<QString, QString> DataTuple;
        typedef QSharedPointer< QList<DataTuple> > List;

        QString html;

        QString searchText;
        int limitPos;

        int typePos;

        int resultPos;
        List results;

        static List newList() { return List( new QList<DataTuple>() ); }
    };

    SimpleSearcher * m_form;
    QList<HistoryNode> m_history;
    int m_historyPos;
};

#endif // SIMPLESEARCHERHISTORY_H
