# norootforbuild
%{?__python3: %global __python %{__python3}}
%if 0%{?suse_version}
%global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")
%else
%{!?python_sitearch: %global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")}
%endif

%define __cmake %{_bindir}/cmake
%define cmake \
CFLAGS="${CFLAGS:-%optflags}" ; export CFLAGS ; \
CXXFLAGS="${CXXFLAGS:-%optflags}" ; export CXXFLAGS ; \
FFLAGS="${FFLAGS:-%optflags}" ; export FFLAGS ; \
%__cmake -DCMAKE_INSTALL_PREFIX:PATH=%{_prefix}

Name:           otagrum
Version:        0.11
Release:        0%{?dist}
Summary:        OpenTURNS module
Group:          System Environment/Libraries
License:        LGPLv3+
URL:            http://www.openturns.org/
Source0:        http://downloads.sourceforge.net/openturns-modules/otagrum/otagrum-%{version}.tar.bz2
BuildRequires:  gcc-c++, cmake, swig >= 4
%if 0%{?suse_version}
BuildRequires:  gcc11-c++
%endif
BuildRequires:  openturns-devel
BuildRequires:  python3-openturns
BuildRequires:  python3-devel
BuildRequires:  agrum-devel
BuildRequires:  python3-agrum
Requires:       libotagrum0

%description
Template module for OpenTURNS library.

%package -n libotagrum0
Summary:        otagrum development files
Group:          Development/Libraries/C and C++
Requires:       libagrum0

%description -n libotagrum0
Dynamic libraries for otagrum.

%package devel
Summary:        otagrum development files
Group:          Development/Libraries/C and C++
Requires:       libotagrum0 = %{version}
Requires:       openturns-devel

%description devel
Development files for otagrum library.

%package -n python3-%{name}
Summary:        otagrum library
Group:          Productivity/Scientific/Math
Requires:       python3-openturns
Requires:       python3-agrum
%description -n python3-%{name}
Python textual interface to otagrum uncertainty library

%prep
%setup -q

%build
%if 0%{?suse_version}
export CXX=/usr/bin/g++-11
%endif
%cmake -DINSTALL_DESTDIR:PATH=%{buildroot} \
       -DCMAKE_SKIP_INSTALL_RPATH:BOOL=ON \
       -DCMAKE_UNITY_BUILD=ON .
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}

%check
LD_LIBRARY_PATH=%{buildroot}%{_libdir} ctest %{?_smp_mflags} --output-on-failure -R pyinstallcheck

%post -n libotagrum0 -p /sbin/ldconfig 
%postun -n libotagrum0 -p /sbin/ldconfig 

%files -n libotagrum0
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
%dir %{_includedir}/%{name}
%{_includedir}/%{name}/*.h*
%{_includedir}/%{name}/swig/
%{_libdir}/*.so
%{_libdir}/cmake/

%files -n python3-%{name}
%defattr(-,root,root,-)
%{python_sitearch}/%{name}/
%{python_sitearch}/%{name}-*.dist-info/


%changelog
* Wed Nov 28 2012 Julien Schueller <schueller at phimeca dot com> 0.0-1
- Initial package creation

