# -*- rpm-spec -*-
BuildRoot:      /home/viktor/OpenDaVINCI/build/odsupercomponent-prefix/src/odsupercomponent-build/_CPack_Packages/Linux/RPM/opendavinci_3.0.2_x86_64/odsupercomponent
Summary:        odsupercomponent is a configuration and monitoring component for distributed software systems realized with OpenDaVINCI.
Name:           opendavinci-odsupercomponent
Version:        3.0.2
Release:        1
License:        GPL
Group:          unknown
Vendor:         Christian Berger

Requires: opendavinci-lib







Prefix: /usr

%define _rpmdir /home/viktor/OpenDaVINCI/build/odsupercomponent-prefix/src/odsupercomponent-build/_CPack_Packages/Linux/RPM
%define _rpmfilename opendavinci_3.0.2_x86_64-odsupercomponent.rpm
%define _unpackaged_files_terminate_build 0
%define _topdir /home/viktor/OpenDaVINCI/build/odsupercomponent-prefix/src/odsupercomponent-build/_CPack_Packages/Linux/RPM




%description
DESCRIPTION
===========

This is an installer created using CPack (http://www.cmake.org). No additional installation instructions provided.



# This is a shortcutted spec file generated by CMake RPM generator
# we skip _install step because CPack does that for us.
# We do only save CPack installed tree in _prepr
# and then restore it in build.
%prep
mv $RPM_BUILD_ROOT "/home/viktor/OpenDaVINCI/build/odsupercomponent-prefix/src/odsupercomponent-build/_CPack_Packages/Linux/RPM/tmpBBroot"

#p build

%install
if [ -e $RPM_BUILD_ROOT ];
then
  rm -rf $RPM_BUILD_ROOT
fi
mv "/home/viktor/OpenDaVINCI/build/odsupercomponent-prefix/src/odsupercomponent-build/_CPack_Packages/Linux/RPM/tmpBBroot" $RPM_BUILD_ROOT

%clean

%post


%postun


%pre


%preun


%files
%defattr(-,root,root,-)
%dir "/usr/man"
"/usr/man/man1*"
"/usr/man/man1/odsupercomponent.1*"
"/usr/bin/odsupercomponent"




%changelog
* Sun Jul 4 2010 Eric Noulard <eric.noulard@gmail.com> - 3.0.2-1
  Generated by CPack RPM (no Changelog file were provided)
