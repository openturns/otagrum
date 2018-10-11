# norootforbuild
%{!?python_sitearch: %global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")}

%define __cmake %{_bindir}/cmake
%define _cmake_lib_suffix64 -DLIB_SUFFIX=64
%define cmake \
CFLAGS="${CFLAGS:-%optflags}" ; export CFLAGS ; \
CXXFLAGS="${CXXFLAGS:-%optflags}" ; export CXXFLAGS ; \
FFLAGS="${FFLAGS:-%optflags}" ; export FFLAGS ; \
%__cmake \\\
-DCMAKE_INSTALL_PREFIX:PATH=%{_prefix} \\\
%if "%{?_lib}" == "lib64" \
%{?_cmake_lib_suffix64} \\\
%endif \
-DBUILD_SHARED_LIBS:BOOL=ON

Name:           otagr
Version:        0.2.0
Release:        0%{?dist}
Summary:        OpenTURNS module
Group:          System Environment/Libraries
License:        GPLv3+
URL:            http://www.openturns.org/
Source0:        http://downloads.sourceforge.net/openturns-modules/otagr/otagr-%{version}.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires:  gcc-c++, cmake, swig
%if 0%{?suse_version}
BuildRequires:  gcc-fortran
%else
BuildRequires:  gcc-gfortran
%endif
BuildRequires:  openturns-devel
BuildRequires:  python-openturns
BuildRequires:  python-devel
Requires:       libotagr0

%description
Template module for OpenTURNS library.

%package -n libotagr0
Summary:        otagr development files
Group:          Development/Libraries/C and C++

%description -n libotagr0
Dynamic libraries for otagr.

%package devel
Summary:        otagr development files
Group:          Development/Libraries/C and C++
Requires:       libotagr0 = %{version}
Requires:       openturns-devel

%description devel
Development files for otagr library.

%package examples
Summary:        otagr examples
Group:          Productivity/Scientific/Math

%description examples
Example files for otagr

%package -n python-%{name}
Summary:        otagr library
Group:          Productivity/Scientific/Math
Requires:       python-openturns
%description -n python-%{name}
Python textual interface to otagr uncertainty library

%prep
%setup -q

%build
%cmake -DINSTALL_DESTDIR:PATH=%{buildroot} \
       -DCMAKE_SKIP_INSTALL_RPATH:BOOL=ON \
       -DUSE_SPHINX=OFF .
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%check
make tests %{?_smp_mflags}
LD_LIBRARY_PATH=%{buildroot}%{_libdir} ctest %{?_smp_mflags} --output-on-failure
rm %{buildroot}%{python_sitearch}/%{name}/*.pyc

%clean
rm -rf %{buildroot}

%post -n libotagr0 -p /sbin/ldconfig 
%postun -n libotagr0 -p /sbin/ldconfig 

%files -n libotagr0
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
%dir %{_includedir}/%{name}
%{_includedir}/%{name}/*.h*
%{_includedir}/%{name}/swig/
%{_libdir}/*.so
%{_libdir}/cmake/

%files examples
%defattr(-,root,root,-)
%dir %{_datadir}/%{name}
%{_datadir}/%{name}/examples/

%files -n python-%{name}
%defattr(-,root,root,-)
%{python_sitearch}/%{name}


%changelog
* Wed Nov 28 2012 Julien Schueller <schueller at phimeca dot com> 0.0-1
- Initial package creation

