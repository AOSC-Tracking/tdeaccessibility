/***************************************************************************
    copyright            : (C) 2004 by Allan Sandfeld Jensen
    email                : kde@carewolf.com
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef AKODEPLAYER_H
#define AKODEPLAYER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "kdeexportfix.h"

#include <tqstring.h>

#include "player.h"
#include <tdemacros.h>
namespace aKode {
    class File;
    class Player;
}

class TDE_EXPORT aKodePlayer : public Player
{
    TQ_OBJECT
  

public:
    aKodePlayer(TQObject* parent = 0, const char* name = 0, const TQStringList& args=TQStringList());
    virtual ~aKodePlayer();

    // virtual void play(const FileHandle &file = FileHandle::null());
    virtual void startPlay(const TQString &file);

    virtual void setVolume(float volume = 1.0);
    virtual float volume() const;

    virtual bool playing() const;
    virtual bool paused() const;

    virtual int totalTime() const;
    virtual int currentTime() const;
    virtual int position() const;

    virtual void seek(int seekTime);
    virtual void seekPosition(int position);

    virtual TQStringList getPluginList( const TQCString& classname );
    virtual void setSinkName(const TQString &sinkName);

public slots:
    void pause();
    void stop();

private:
    aKode::Player *m_player;
    TQString m_sinkName;
};

#endif
