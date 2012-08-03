/*
** Copyright (C) 2009-2012 Quadrant Information Security <quadrantsec.com>
** Copyright (C) 2009-2012 Champ Clark III <cclark@quadrantsec.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* sagan-output.c 
* 
* This becomes a threaded operation.  This handles all I/O intensive output plugins
*/
#ifdef HAVE_CONFIG_H
#include "config.h"             /* From autoconf */
#endif

#include <stdio.h>
#include <pthread.h>

#include "sagan.h"

struct _SaganCounters *counters;
struct _Rule_Struct *rulestruct;

void sagan_output( _SaganEvent *Event )
{

pthread_mutex_t counters_mutex = PTHREAD_MUTEX_INITIALIZER;

/****************************************************************************/
/* Snortsam Support	                                                    */
/****************************************************************************/

/* If we have a snortsam server && the rule requires snortsam..... */

#ifdef WITH_SNORTSAM
if ( Event->config->sagan_fwsam_flag && rulestruct[Event->found].fwsam_src_or_dst ) sagan_fwsam( Event );
#endif


/****************************************************************************/
/* Snort DB/SQL support                                                     */
/****************************************************************************/

#if defined(HAVE_LIBMYSQLCLIENT_R) || defined(HAVE_LIBPQ)
if ( Event->config->dbtype != 0 ) sagan_db_thread( Event );
#endif

/****************************************************************************/
/* SMTP/Email support (libesmtp)                                            */
/****************************************************************************/

#ifdef HAVE_LIBESMTP
if ( Event->config->sagan_esmtp_flag ) sagan_esmtp_thread( Event ); 
#endif

/****************************************************************************/
/* Prelude framework thread call (libprelude                                */
/****************************************************************************/

#if HAVE_LIBPRELUDE
if ( Event->config->sagan_prelude_flag ) sagan_prelude( Event );
#endif

/****************************************************************************/
/* External program support                                                 */
/****************************************************************************/

if ( Event->config->sagan_ext_flag ) sagan_ext_thread( Event );


pthread_mutex_lock(&counters_mutex);
counters->thread_output_counter--;
pthread_mutex_unlock(&counters_mutex);

pthread_exit(NULL);
}
