#include "movementsIO.h"
#include <accountbaseplugin/constants.h>
#include <accountbaseplugin/availablemovementmodel.h>
#include <accountbaseplugin/movementmodel.h>
#include <accountbaseplugin/bankaccountmodel.h>
#include <coreplugin/icore.h>
#include <coreplugin/itheme.h>
#include <coreplugin/iuser.h>
#include <coreplugin/constants_icons.h>
#include <utils/database.h>
#include <QMessageBox>
#include <QDebug>
#include <QDate>

using namespace AccountDB;
using namespace Constants;
using namespace Utils;
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Core::IUser *user() { return  Core::ICore::instance()->user(); }
MovementsIODb::MovementsIODb(QObject *parent) :
        QObject(parent)
{
    m_modelMovements = new MovementModel(parent);
    for (int i = 0; i < Constants::MOV_MaxParam; i += 1)
    {
    	Database db;
    	QString value = db.fieldName(Constants::Table_Movement, i) ;
    	//qDebug() << __FILE__ << QString::number(__LINE__) << " value =" << value ;
    	m_modelMovements->setHeaderData(i,Qt::Horizontal,value,Qt::EditRole);
        }
    m_user_uid = user()->value(Core::IUser::Uuid).toString();
}

MovementsIODb::~MovementsIODb()
{
}

MovementModel *MovementsIODb::getModelMovements(QString &year)
{qDebug() << __FILE__ << QString::number(__LINE__) << " year =" << year ;
    QString filter = QString("DATEVALUE between '%1' AND '%2'").arg(year+"-01-01",year+"-12-31");
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
    QString filterUid = m_modelMovements->filter();
    QString dateAndUidFilter = filterUid+" AND "+filter;
    m_modelMovements->setFilter(dateAndUidFilter);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << m_modelMovements->filter() ;
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowCount =" << QString::number(m_modelMovements->rowCount()) ;
    return m_modelMovements;
}

QString MovementsIODb::getUserUid(){
    return m_modelMovements->m_UserUid;
}

QStringList MovementsIODb::listOfParents(){
    QStringList list;
    AvailableMovementModel availablemodel(this);
    for (int i = 0; i < availablemodel.rowCount(); i += 1)
    {
    	QString parent = availablemodel.data(availablemodel.index(i,AVAILMOV_PARENT),Qt::DisplayRole).toString();
    	list << parent;
        }
    return list;
}

QHash<QString,QString> MovementsIODb::hashChildrenAndParentsAvailableMovements(){
    QHash<QString,QString> hash;
    AvailableMovementModel availablemodel(this);
    int rows = availablemodel.rowCount();
    for (int i = 0; i < rows; i += 1)
    {
    	QString child =  availablemodel.data(availablemodel.index(i,AVAILMOV_LABEL),Qt::DisplayRole).toString();   	
    	QString parent = availablemodel.data(availablemodel.index(i,AVAILMOV_PARENT),Qt::DisplayRole).toString();
    	hash.insertMulti(child,parent);
        }
    return hash;
}

QStandardItemModel  *MovementsIODb::getMovementsComboBoxModel(QObject *parent)
{
    QStandardItemModel *model = new QStandardItemModel(parent);
    AvailableMovementModel availablemodel(this);
    QStringList listOfAvModelParents;
    listOfAvModelParents = listOfParents();
    for (int i = 0; i < availablemodel.rowCount(); i += 1) {
        //todo : supprimer l'affichage des parents qui seront affichés en tool tip avec les commentaires
    	int type = availablemodel.data(availablemodel.index(i,AVAILMOV_TYPE),Qt::DisplayRole).toInt();
    	QIcon icon;
        if (type == 1) {
            icon = QIcon(theme()->icon(Core::Constants::ICONADD));
        } else {
            icon = QIcon(theme()->icon(Core::Constants::ICONREMOVE));
    	}
    	QString label = availablemodel.data(availablemodel.index(i,AVAILMOV_LABEL),Qt::DisplayRole).toString();
    	QStandardItem *item = new QStandardItem(icon,label);
    	if (!listOfAvModelParents.contains(label))
    	{
    		  model->appendRow(item);// no parents in the list of items
    	    }
    	
        }
    return model;
}

QStringList MovementsIODb::getYearComboBoxModel()
{
    QStringList listOfYears;
    for (int i = 0; i < m_modelMovements->rowCount(); i += 1) {
    	QString dateStr = m_modelMovements->data(m_modelMovements->index(i,MOV_DATE),Qt::DisplayRole).toString();
    	QString dateOfValueStr = m_modelMovements->data(m_modelMovements->index(i,MOV_DATE),Qt::DisplayRole).toString();
    	QString yearDate = QString::number(QDate::fromString(dateStr,"yyyy-MM-dd").year());
    	QString yearDateOfValue = QString::number(QDate::fromString(dateOfValueStr,"yyyy-MM-dd").year());
    	listOfYears << yearDate << yearDateOfValue;
    }
    listOfYears.removeDuplicates();
    return listOfYears;
}

QStandardItemModel * MovementsIODb::getBankComboBoxModel(QObject * parent){
    //todo preferentiel bank
    QStandardItemModel *model = new QStandardItemModel(parent);
    BankAccountModel bankmodel(this);
    QString filterUserAndPrefered = QString("BD_USER_UID = '%1' AND BD_ISDEFAULT = '%2'").arg(m_user_uid,1);
    QString filterUser = QString("BD_USER_UID = '%1'").arg(m_user_uid);
    int rows = bankmodel.rowCount();
    for (int i = 0; i < rows; i += 1)
    {
    	QString bankLabel = bankmodel.data(bankmodel.index(i,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
    	QString bankDefault = bankmodel.data(bankmodel.index(i,BANKDETAILS_DEFAULT),Qt::DisplayRole).toString();
    	QStandardItem *item = new QStandardItem(bankLabel);
    	QIcon icon;
        if (bankDefault == "1") {
            icon = QIcon(theme()->icon(Core::Constants::ICONADD));
            item->setIcon(icon);
            qDebug() << __FILE__ << QString::number(__LINE__) << " item def =" << item->text() ;
            model->appendRow(item);
            } 
        }
    for (int i = 0; i < rows; i += 1)
    {
    	QString bankLabel = bankmodel.data(bankmodel.index(i,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
    	QString bankDefault = bankmodel.data(bankmodel.index(i,BANKDETAILS_DEFAULT),Qt::DisplayRole).toString();
    	QStandardItem *item = new QStandardItem(bankLabel);
    	QIcon icon;   
    	if (bankDefault != "1")
    	{
    	    icon = QIcon(theme()->icon(Core::Constants::ICONREMOVE));
            item->setIcon(icon);
            qDebug() << __FILE__ << QString::number(__LINE__) << " item def =" << item->text() ;
            model->appendRow(item);
    	    } 
        }
    return model;
}

bool MovementsIODb::insertIntoMovements(QHash<int,QVariant> &hashValues)
{
    bool ret = true;
    double value = 0.00;
    int type = 2;
    QString bank;//todo : find bank label with accountId
    int rowBefore = m_modelMovements->rowCount(QModelIndex());
    qDebug() << __FILE__ << QString::number(__LINE__) << " rowBefore = " << QString::number(rowBefore);
    if (m_modelMovements->insertRows(rowBefore,1,QModelIndex())) {
        qWarning() << __FILE__ << QString::number(__LINE__) << "Row inserted !" ;
    }
    QVariant data;
    for(int i = 1 ; i < MOV_MaxParam ; i ++) {
        data = hashValues.value(i);
        if (i == MOV_AMOUNT)
        {
        	 value = data.toDouble(); 
            }
        if (i == MOV_TYPE)
        {
        	  type = data.toInt();
            }
        if (i == MOV_ACCOUNT_ID)
        {
        	  int bankId = data.toInt();
        	  bank = getBankNameFromId(bankId);
        	  qDebug() << __FILE__ << QString::number(__LINE__) << " bank =" << bank ;
            }
        qDebug() << __FILE__ << QString::number(__LINE__) << " data + i =" << data.toString()+" "+QString::number(i);
        if (!m_modelMovements-> setData(m_modelMovements->index(rowBefore,i), data ,Qt::EditRole)) {
            qWarning() << __FILE__ << QString::number(__LINE__) << " model account error = "
                    << m_modelMovements->lastError().text() ;
        }
    }
    m_modelMovements->submit();
    if (m_modelMovements->rowCount(QModelIndex()) == rowBefore) {
        QMessageBox::warning(0,trUtf8("Warning"),__FILE__+QString::number(__LINE__)
                             + trUtf8("\nError = ") 
                             + m_modelMovements->lastError().text(),
                             QMessageBox::Ok);
        ret = false;
    }
    if (type == 0)
    {
    	  value = 0.00 - value;
    	  qDebug() << __FILE__ << QString::number(__LINE__) << " value neg =" << QString::number(value) ;
    	  
        }
    if (!debitOrCreditInBankBalance(bank,value)){
    	  	  qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to debit or credit balance !" ;
    	}
    return ret;
}

bool MovementsIODb::deleteMovement(int row)
{qDebug() << __FILE__ << QString::number(__LINE__) << " row =" << QString::number(row) ;
    bool b = m_modelMovements->removeRows(row,1,QModelIndex());
    return b;
}

int MovementsIODb::getAvailableMovementId(QString &movementsComboBoxText)
{
    int availableMovementId = 0;
    AvailableMovementModel  availablemodel(this);
    Database db;
    QString field = availablemodel.headerData(AVAILMOV_LABEL,Qt::Horizontal,Qt::DisplayRole).toString() ;
    QString filter = field +QString(" = '%1'").arg(movementsComboBoxText);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
    availablemodel.setFilter(filter);
    availableMovementId = availablemodel.data(availablemodel.index(0,AVAILMOV_ID),Qt::DisplayRole).toInt();
    return availableMovementId;
}

int MovementsIODb::getBankId(QString & bankComboBoxText){
    int bankId = 0;
    BankAccountModel model(this);
    Database db;
    QString field = model.headerData(BANKDETAILS_LABEL,Qt::Horizontal,Qt::DisplayRole).toString();
    QString filter = field +QString(" = '%1'").arg(bankComboBoxText);
    model.setFilter(filter);
    bankId = model.data(model.index(0,BANKDETAILS_ID),Qt::DisplayRole).toInt();
    return bankId;
}

QString MovementsIODb::getBankNameFromId(int id){
    QString bank;
    BankAccountModel model(this);
    QString field = model.headerData(BANKDETAILS_ID,Qt::Horizontal,Qt::DisplayRole).toString();
    QString filter = field +QString(" = '%1'").arg(id);
    model.setFilter(filter);
    qDebug() << __FILE__ << QString::number(__LINE__) << " model filter =" << model.filter() ;
    bank = model.data(model.index(0,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
    return bank;
}

int MovementsIODb::getTypeOfMovement(QString & movementsComboBoxText){
    int type = 0;
    AvailableMovementModel  availablemodel(this);
    QString field = availablemodel.headerData(AVAILMOV_LABEL,Qt::Horizontal,Qt::DisplayRole).toString() ;
    QString filter = field +QString(" = '%1'").arg(movementsComboBoxText);
    qDebug() << __FILE__ << QString::number(__LINE__) << " filter =" << filter ;
    availablemodel.setFilter(filter);
    type = availablemodel.data(availablemodel.index(0,AVAILMOV_TYPE),Qt::DisplayRole).toInt();
    return type;
}

bool MovementsIODb::validMovement(int row)
{
    return m_modelMovements->setData(m_modelMovements->index(row, MOV_ISVALID), 1, Qt::EditRole);
}

bool MovementsIODb::debitOrCreditInBankBalance(const QString & bank, double & value){
    bool ret = true;
    BankAccountModel model(this);
    int row = 0;
    QList<int> rowsTestList;
    for (int i = 0; i < model.rowCount(); i += 1)
    {
    	QString bankLabel = model.data(model.index(i,BANKDETAILS_LABEL),Qt::DisplayRole).toString();
    	if (bankLabel == bank)
    	{
    		  row = i;
    		  rowsTestList << i;
    	    }
        }
    if (rowsTestList.size()>1)
    {
    	  QMessageBox::warning(0,trUtf8("Warning"),
    	             trUtf8("You have two or more records with the same bank name ! Risk of errors!"),QMessageBox::Ok);
        }
    double balance = model.data(model.index(row,BANKDETAILS_BALANCE),Qt::DisplayRole).toDouble();
    double newBalance = balance + value;
    QDate date = QDate::currentDate();
    if (!model.setData(model.index(row,BANKDETAILS_BALANCE),newBalance,Qt::EditRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to insert balance data !" ;
        }
    if (!model.setData(model.index(row,BANKDETAILS_BALANCEDATE),date,Qt::EditRole))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "Unable to insert balance new date !" ;
        }
    if (!model.submit())
    {
    	  ret = false;
        }
    return ret;
}
