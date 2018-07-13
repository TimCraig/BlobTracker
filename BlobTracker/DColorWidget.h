/*
 * DColorWidget.h
 *
 *  Created on: Jul 6, 2014
 *      Author: Tim
 */

#ifndef DCOLORWIDGET_H_
#define DCOLORWIDGET_H_

/*****************************************************************************
 ******************************  I N C L U D E  ******************************
 ****************************************************************************/

#include <QWidget>
#include <QColor>

/*****************************************************************************
 *
 ***  class DColorWidget
 *
 *****************************************************************************/

class DColorWidget : public QWidget
   {
      Q_OBJECT

   public:
      DColorWidget(bool bAllowEdit, QWidget* pParent = nullptr);
      DColorWidget(const DColorWidget& src) = delete;

      ~DColorWidget() = default;

      DColorWidget& operator=(const DColorWidget& rhs) = delete;

      QColor GetColor() const
         {
         return m_Background;
         }

   signals:
      void ColorChanged(const QColor& Color);

   public slots:
      void SetColor(const QColor& Color);

      void EnableEditing();
      void DisableEditing();

   protected:
      bool m_bAllowEdit;
      QColor m_Background;

      void paintEvent(QPaintEvent* pEvent);

      // Used for invoking editing
      void mousePressEvent(QMouseEvent* pEvent);

   private:

   }; // end of class DColorWidget

#endif /* DCOLORWIDGET_H_ */
