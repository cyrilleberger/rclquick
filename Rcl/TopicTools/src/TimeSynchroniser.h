#pragma once

#include <QObject>

#include <QMutex>

#include <QQmlListProperty>

class TimeSynchroniser : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QQmlListProperty<QObject> subscribers READ subscribers)
public:
  TimeSynchroniser(QObject* _parent = nullptr);
  ~TimeSynchroniser();
private:
  static void appendSubscriber(QQmlListProperty<QObject>* _property, QObject* _subscriber);
  static int subscriberCount(QQmlListProperty<QObject>* _property);
  static QObject* subscriber(QQmlListProperty<QObject>* _property, int _index);
  static void clearSubscribers(QQmlListProperty<QObject>* _property);
  QQmlListProperty<QObject> subscribers();
private slots:
  void handleMessage(const QVariant& messages);
signals:
  void messagesReceived(const QVariant& messages);
private:
  QMutex m_mutex;
  QList<QObject*> m_subscribers;
  QMap<QObject*, QVariantList> m_messages;
};
