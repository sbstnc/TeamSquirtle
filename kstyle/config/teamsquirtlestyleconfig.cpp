
/*************************************************************************
 * Copyright (C) 2014 by Hugo Pereira Da Costa <hugo.pereira@free.fr>    *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *************************************************************************/

#include "teamsquirtlestyleconfig.h"

#include "../teamsquirtle.h"
#include "../config-teamsquirtle.h"
#include "teamsquirtlestyleconfigdata.h"

#include <QDBusMessage>
#include <QDBusConnection>

#if !TEAMSQUIRTLE_USE_KDE4
#include <Kdelibs4Migration>
#endif

extern "C"
{
    Q_DECL_EXPORT QWidget* allocate_kstyle_config(QWidget* parent)
    { return new TeamSquirtle::StyleConfig(parent); }
}

namespace TeamSquirtle
{

    //__________________________________________________________________
    StyleConfig::StyleConfig(QWidget* parent):
        QWidget(parent)
    {
        setupUi(this);

        // load setup from configData
        load();

        connect( _tabBarDrawCenteredTabs, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _toolBarDrawItemSeparator, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _viewDrawFocusIndicator, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _dockWidgetDrawFrame, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _titleWidgetDrawFrame, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _sidePanelDrawFrame, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _menuItemDrawThinFocus, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _sliderDrawTickMarks, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _splitterProxyEnabled, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _mnemonicsMode, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _animationsEnabled, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _animationsDuration, SIGNAL(valueChanged(int)), SLOT(updateChanged()) );
        connect( _scrollBarAddLineButtons, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _scrollBarSubLineButtons, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _windowDragMode, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );

    }

    //__________________________________________________________________
    void StyleConfig::save( void )
    {
        StyleConfigData::setTabBarDrawCenteredTabs( _tabBarDrawCenteredTabs->isChecked() );
        StyleConfigData::setToolBarDrawItemSeparator( _toolBarDrawItemSeparator->isChecked() );
        StyleConfigData::setViewDrawFocusIndicator( _viewDrawFocusIndicator->isChecked() );
        StyleConfigData::setDockWidgetDrawFrame( _dockWidgetDrawFrame->isChecked() );
        StyleConfigData::setTitleWidgetDrawFrame( _titleWidgetDrawFrame->isChecked() );
        StyleConfigData::setSidePanelDrawFrame( _sidePanelDrawFrame->isChecked() );
        StyleConfigData::setMenuItemDrawStrongFocus( !_menuItemDrawThinFocus->isChecked() );
        StyleConfigData::setSliderDrawTickMarks( _sliderDrawTickMarks->isChecked() );
        StyleConfigData::setSplitterProxyEnabled( _splitterProxyEnabled->isChecked() );
        StyleConfigData::setMnemonicsMode( _mnemonicsMode->currentIndex() );
        StyleConfigData::setScrollBarAddLineButtons( _scrollBarAddLineButtons->currentIndex() );
        StyleConfigData::setScrollBarSubLineButtons( _scrollBarSubLineButtons->currentIndex() );
        StyleConfigData::setAnimationsEnabled( _animationsEnabled->isChecked() );
        StyleConfigData::setAnimationsDuration( _animationsDuration->value() );
        StyleConfigData::setWindowDragMode( _windowDragMode->currentIndex()  );

        #if TEAMSQUIRTLE_USE_KDE4
        StyleConfigData::self()->writeConfig();
        #else
        StyleConfigData::self()->save();

        //update the KDE4 config to match
        Kdelibs4Migration migration;
        const QString kde4ConfigDirPath = migration.saveLocation("config");

        QScopedPointer<KConfig> kde4Config(StyleConfigData::self()->config()->copyTo(kde4ConfigDirPath+"/teamsquirtlerc", 0));
        kde4Config->sync();
        #endif

        // emit dbus signal
        QDBusMessage message( QDBusMessage::createSignal( QStringLiteral( "/TeamSquirtleStyle" ),  QStringLiteral( "org.kde.TeamSquirtle.Style" ), QStringLiteral( "reparseConfiguration" ) ) );
        QDBusConnection::sessionBus().send(message);

    }

    //__________________________________________________________________
    void StyleConfig::defaults( void )
    {
        StyleConfigData::self()->setDefaults();
        load();
    }

    //__________________________________________________________________
    void StyleConfig::reset( void )
    {
        // reparse configuration
        #if TEAMSQUIRTLE_USE_KDE4
        StyleConfigData::self()->readConfig();
        #else
        StyleConfigData::self()->load();
        #endif

        load();
    }

    //__________________________________________________________________
    void StyleConfig::updateChanged()
    {

        bool modified( false );

        // check if any value was modified
        if( _tabBarDrawCenteredTabs->isChecked() != StyleConfigData::tabBarDrawCenteredTabs() ) modified = true;
        else if( _toolBarDrawItemSeparator->isChecked() != StyleConfigData::toolBarDrawItemSeparator() ) modified = true;
        else if( _viewDrawFocusIndicator->isChecked() != StyleConfigData::viewDrawFocusIndicator() ) modified = true;
        else if( _dockWidgetDrawFrame->isChecked() != StyleConfigData::dockWidgetDrawFrame() ) modified = true;
        else if( _titleWidgetDrawFrame->isChecked() != StyleConfigData::titleWidgetDrawFrame() ) modified = true;
        else if( _sidePanelDrawFrame->isChecked() != StyleConfigData::sidePanelDrawFrame() ) modified = true;
        else if( _menuItemDrawThinFocus->isChecked() == StyleConfigData::menuItemDrawStrongFocus() ) modified = true;
        else if( _sliderDrawTickMarks->isChecked() != StyleConfigData::sliderDrawTickMarks() ) modified = true;
        else if( _mnemonicsMode->currentIndex() != StyleConfigData::mnemonicsMode() ) modified = true;
        else if( _scrollBarAddLineButtons->currentIndex() != StyleConfigData::scrollBarAddLineButtons() ) modified = true;
        else if( _scrollBarSubLineButtons->currentIndex() != StyleConfigData::scrollBarSubLineButtons() ) modified = true;
        else if( _splitterProxyEnabled->isChecked() != StyleConfigData::splitterProxyEnabled() ) modified = true;
        else if( _animationsEnabled->isChecked() != StyleConfigData::animationsEnabled() ) modified = true;
        else if( _animationsDuration->value() != StyleConfigData::animationsDuration() ) modified = true;
        else if( _windowDragMode->currentIndex() != StyleConfigData::windowDragMode() ) modified = true;

        emit changed(modified);

    }

    //__________________________________________________________________
    void StyleConfig::load( void )
    {

        _tabBarDrawCenteredTabs->setChecked( StyleConfigData::tabBarDrawCenteredTabs() );
        _toolBarDrawItemSeparator->setChecked( StyleConfigData::toolBarDrawItemSeparator() );
        _viewDrawFocusIndicator->setChecked( StyleConfigData::viewDrawFocusIndicator() );
        _dockWidgetDrawFrame->setChecked( StyleConfigData::dockWidgetDrawFrame() );
        _titleWidgetDrawFrame->setChecked( StyleConfigData::titleWidgetDrawFrame() );
        _sidePanelDrawFrame->setChecked( StyleConfigData::sidePanelDrawFrame() );
        _menuItemDrawThinFocus->setChecked( !StyleConfigData::menuItemDrawStrongFocus() );
        _sliderDrawTickMarks->setChecked( StyleConfigData::sliderDrawTickMarks() );
        _mnemonicsMode->setCurrentIndex( StyleConfigData::mnemonicsMode() );
        _splitterProxyEnabled->setChecked( StyleConfigData::splitterProxyEnabled() );
        _scrollBarAddLineButtons->setCurrentIndex( StyleConfigData::scrollBarAddLineButtons() );
        _scrollBarSubLineButtons->setCurrentIndex( StyleConfigData::scrollBarSubLineButtons() );
        _animationsEnabled->setChecked( StyleConfigData::animationsEnabled() );
        _animationsDuration->setValue( StyleConfigData::animationsDuration() );
        _windowDragMode->setCurrentIndex( StyleConfigData::windowDragMode() );

    }

}
