#!/bin/sh
[ "$BASH" ] || exec bash $0 "$@"
#
# $Id$
#

# ---------------------------------------------------------------
# Copyright 2010 Viktor Szakats (harbour.01 syenar.hu)
# See COPYING for licensing terms.
# ---------------------------------------------------------------

#
# HOWTO upload unattended to sourceforge.net
# ==========================================
#
# 1. Generate SSH key pair:
#
#    $ ssh-keygen -t dsa -C "$HB_SFNET_USER,harbour-project@frs.sourceforge.net"
#
#    where '$HB_SFNET_USER' is your sf.net username.
#
#    IMPORTANT: Leave pass-phrase empty
#
# 2. Paste content of generated public key file (by default 'id_dsa.pub') into
#    'Authorized keys' edit box and press 'Update' button on this page:
#
#    https://sourceforge.net/account/ssh
#
#    You have to be logged in to sf.net.
#
# 3. Wait up to 10 minutes for key to be activated
#
# 4. Now you can use your private key file (by default 'id_dsa') instead of
#    password to upload files. To use this script as is, set 'HB_SFNET_FRS_PRIVATE_KEY'
#    envvar to your private key file and 'HB_SFNET_USER' envvar to your
#    sf.net username, f.e.:
#
#    $ export HB_SFNET_FRS_PRIVATE_KEY=~/.ssh/id_dsa
#    $ export HB_SFNET_USER=myuser
#    $ ./mpkg_nightly.sh
#
# Official link with more detailed procedures:
#    http://sourceforge.net/apps/trac/sourceforge/wiki/SSH%20keys
#

[ -n harbour-nightly-src.zip ] || rm harbour-nightly-src.zip
[ -n harbour-nightly.tar.bz2 ] || rm harbour-nightly.tar.bz2
[ -n harbour-nightly.tar.gz ]  || rm harbour-nightly.tar.gz

rm -f -r _mk_nightly
mkdir _mk_nightly
cd _mk_nightly

rm -f -r harbour
svn export --native-eol LF http://harbour-project.svn.sourceforge.net/svnroot/harbour-project/trunk/harbour
tar -c harbour/* > harbour-nightly.tar
bzip2 -c -z harbour-nightly.tar > ../harbour-nightly.tar.bz2
gzip -c harbour-nightly.tar > ../harbour-nightly.tar.gz
rm harbour-nightly.tar

rm -f -r harbour
svn export --native-eol CRLF http://harbour-project.svn.sourceforge.net/svnroot/harbour-project/trunk/harbour
zip -X -r -o ../harbour-nightly-src.zip harbour/*

cd ..
rm -f -r _mk_nightly

scp -i $HB_SFNET_FRS_PRIVATE_KEY harbour-nightly-src.zip $HB_SFNET_USER,harbour-project@frs.sourceforge.net:/home/frs/project/h/ha/harbour-project/source/nightly/
scp -i $HB_SFNET_FRS_PRIVATE_KEY harbour-nightly.tar.bz2 $HB_SFNET_USER,harbour-project@frs.sourceforge.net:/home/frs/project/h/ha/harbour-project/source/nightly/
scp -i $HB_SFNET_FRS_PRIVATE_KEY harbour-nightly.tar.gz  $HB_SFNET_USER,harbour-project@frs.sourceforge.net:/home/frs/project/h/ha/harbour-project/source/nightly/
