#include "simplesearcherhistory.h"
#include "simplesearcher.h"
#include "ui_simplesearcher.h"

#include <QWebFrame>
#include <QDebug>

SimpleSearcherHistory::
SimpleSearcherHistory(SimpleSearcher * s) :
    QObject(s),
    m_form(s),
    m_history(),
    m_historyPos(-1)
{}


bool SimpleSearcherHistory::
loadPreviousData()
{
    if( ! hasPrevious() ) return false;

    restore(m_historyPos-1);

    return true;
}

bool SimpleSearcherHistory::
loadNextData()
{
    if( ! hasNext() ) return false;

    restore(m_historyPos+1);

    return true;
}

void SimpleSearcherHistory::
saveData()
{
    if( m_historyPos + 1 != m_history.count() )
        remove( m_historyPos + 1 );

    int typeIndex   = m_form->ui->typeComboBox->currentIndex();
    int resultIndex = m_form->ui->resultComboBox->currentIndex();

    if( typeIndex < 0 || resultIndex < 0 )
        return;

    HistoryNode n;
    if( m_history.count() )
        n = m_history.last();

    n.html = m_form->webView()->page()->mainFrame()->toHtml();

    if( typeIndex == n.typePos && n.searchText == m_form->ui->searchLineEdit->text() )
    {
        n.resultPos = resultIndex;
    }
    else {

        QComboBox * resultComboBox = m_form->ui->resultComboBox;

        n.searchText = m_form->ui->searchLineEdit->text();
        n.limitPos   = m_form->ui->limitComboBox->currentIndex();

        n.typePos    = m_form->ui->typeComboBox->currentIndex();

        n.resultPos  = resultComboBox->currentIndex();
        n.results    = HistoryNode::newList();

        int count = resultComboBox->count();

        for( int i(0); i < count; ++i )
        {
            n.results->append(
                HistoryNode::DataTuple(
                    resultComboBox->itemText(i),
                    resultComboBox->itemData(i).toString()
                )
            );
        }
    }

    ++m_historyPos;
    m_history.append( n );

    m_form->ui->prevPageButton->setEnabled( hasPrevious() );
    m_form->ui->nextPageButton->setEnabled( hasNext() );
}

void SimpleSearcherHistory::
remove(int start)
{
    int count( m_history.count() - start );
    if( count < 1 ) return;

    while(count--) m_history.pop_back();
}

void SimpleSearcherHistory::
restore(int pos)
{
    m_historyPos = pos;

    const HistoryNode & n = m_history.at(pos);

    QComboBox * resultComboBox = m_form->ui->resultComboBox;
    QComboBox * typeComboBox   = m_form->ui->typeComboBox;

    resultComboBox->blockSignals(true);
    typeComboBox->blockSignals(true);

    m_form->ui->webView->setHtml( n.html );
    m_form->ui->searchLineEdit->setText( n.searchText );
    m_form->ui->limitComboBox->setCurrentIndex( n.limitPos );

    typeComboBox->setCurrentIndex( n.typePos );

    resultComboBox->clear();

    foreach( const HistoryNode::DataTuple & tuple, *(n.results.data()) )
        resultComboBox->addItem(tuple.first, tuple.second);

    resultComboBox->setCurrentIndex( n.resultPos );

    resultComboBox->blockSignals(false);
    typeComboBox->blockSignals(false);

    m_form->ui->prevPageButton->setEnabled( hasPrevious() );
    m_form->ui->nextPageButton->setEnabled( hasNext() );
}
