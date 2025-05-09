Name: mtx
Version: 1.3.11
Release: 1
Summary: SCSI media changer control program
License: GPL
Group: Utilities/System
Source0: ftp://ftp.opensource-sw.net/pub/mtx/development/%{name}-%{version}.tar.gz
Url: http://%{name}.sourceforge.net
BuildRoot: /var/tmp/%{name}-%{version}


%description
The MTX program controls the robotic mechanism in autoloaders and tape
libraries such as the HP SureStore DAT 40x6, Exabyte EZ-17, and
Exabyte 220. This program is also reported to work with a variety of other tape
libraries and autochangers from Tandberg/Overland, Breece Hill, HP, and 
Seagate.

%prep
%setup -q

%build
%configure
make

%install
mkdir -p $RPM_BUILD_ROOT/sbin
install mtx $RPM_BUILD_ROOT/sbin/mtx
mkdir -p $RPM_BUILD_ROOT/usr/sbin
install loaderinfo $RPM_BUILD_ROOT/usr/sbin/loaderinfo
install scsieject $RPM_BUILD_ROOT/usr/sbin/scsieject
install scsitape $RPM_BUILD_ROOT/usr/sbin/scsitape
install tapeinfo $RPM_BUILD_ROOT/usr/sbin/tapeinfo
mkdir -p $RPM_BUILD_ROOT/%{_mandir}/man1
install mtx.1 $RPM_BUILD_ROOT/%{_mandir}/man1/mtx.1
install loaderinfo.1 $RPM_BUILD_ROOT/%{_mandir}/man1/loaderinfo.1
install scsieject.1 $RPM_BUILD_ROOT/%{_mandir}/man1/scsieject.1
install scsitape.1 $RPM_BUILD_ROOT/%{_mandir}/man1/scsitape.1
install tapeinfo.1 $RPM_BUILD_ROOT/%{_mandir}/man1/tapeinfo.1


%clean
rm -rf $RPM_BUILD_ROOT

%files 
%defattr(-,root,root)
%doc mtx.doc CHANGES README mtxl.README.html
%doc COMPATABILITY FAQ LICENSE* TODO contrib
%{_mandir}/man1/*
/sbin/mtx
/usr/sbin/*

%changelog
* Fri Sep 27 2002 Eric Green <eric@badtux.org>
- 1.3.0rel
- move changelog to end.
- change source directory to ftp.badtux.net. 
- use * for files to catch new files. 

* Wed Apr 18 2001 Kenneth Porter <shiva@well.com>
- 1.2.12pre1
- Need to create usr/sbin for install

* Fri Mar 02 2001 Eric Green <eric@estinc.com>
- 1.2.11pre6 
- Move tapeinfo,loaderinfo, scsitape to /usr/sbin rather than /sbin

* Wed Feb 28 2001 Kenneth Porter <shiva@well.com>
- 1.2.11pre5
- Remove commented-out patch.
- Use mandir FHS macro and configure macro.
- Install more stuff.
- Use build policy for stripping.

* Wed Jan 17 2001 Eric Green <eric@estinc.com>
- 1.2.11pre3
- Removed patch, now use ./configure. 

* Mon Nov 27 2000 Eric Green <eric@estinc.com>
- 1.2.10
- Fixed patching to use the portable.patch.

* Tue Jul 25 2000 Eric Green <eric@estinc.com>
- 1.2.8
- Added portability patch to mtx.spec so should compile on Red Hat Alpha etc. 

* Thu Jun 6 2000 Eric Green <eric@estinc.com>
- 1.2.7
- Fixed single-drive Exabyte 220 special case.
- Fixed ADIC DAT Autochanger special case.
- Fixed mtx.spec to move the binaries to /sbin since we need root access

* Fri May 12 2000 Kenneth Porter <shiva@well.com>
- 1.2.6
- Fixed 'eepos' stuff to use | rather than || (whoops!)
- Accept a 4-byte element descriptor for the robot arm for certain older
- autochangers. 

* Mon May 8 2000 Kenneth Porter <shiva@well.com>
- Spell sourceforge right so the link at rpmfind.net will work.

* Thu May 4 2000 Kenneth Porter <shiva@well.com>
- 1.2.5

* Thu Oct 29 1998  Ian Macdonald <ianmacd@xs4all.nl>
- moved mtx from /sbin to /bin, seeing as mt is also located there

* Fri Oct 23 1998  Ian Macdonald <ianmacd@xs4all.nl>
- first RPM release
