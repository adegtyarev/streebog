Name:		GOST34.11-2012
Version:	0.12
Release:	1%{?dist}
Summary:	RFC-6986 cryptographic hash function
Group:		Productivity/Security

License:	BSD-2-Clause
URL:		https://streebog.net
Source0:	https://github.com/adegtyarev/streebog/archive/%{version}.tar.gz

BuildRequires:	gcc
BuildRequires:	help2man
ExclusiveArch:	x86_64 i386 i586 i686 ppc ppc64 ppc64le aarch64 armv6l armv6hl armv7l armv7hl s390x

%description
Command line tool to create GOST R 34.11-2012 hash digest of files or streams.

Russian cryptographic standard GOST R 34.11-2012 establishes the hash-function
algorithm and the hash-function calculation procedure for any sequence of
binary symbols. The standard published as RFC 6986.

%package devel
Summary:	Header files for the RFC-6986 cryptographic hash function
Group:		Development/Languages/C and C++

%description devel
Header files for the GOST R 34.11-2012 hash digest of files or streams.

Russian cryptographic standard GOST R 34.11-2012 establishes the hash-function
algorithm and the hash-function calculation procedure for any sequence of
binary symbols. The standard published as RFC 6986.

%prep
%setup -q -n streebog-%{version}

%build
make %{?_smp_mflags} config build man

%install
mkdir -p %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_mandir}/man1
mkdir -p %{buildroot}%{_includedir}/gost3411-2012
install -pm 755 gost3411-2012 %{buildroot}%{_bindir}/
install -pm 644 gost3411-2012.1 %{buildroot}%{_mandir}/man1/
install -pm 644 gost3411-2012-const.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-core.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-mmx.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-precalc.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-ref.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-sse2.h %{buildroot}%{_includedir}/gost3411-2012/
install -pm 644 gost3411-2012-sse41.h %{buildroot}%{_includedir}/gost3411-2012/

%files
%defattr(-, root, root)
%{_bindir}/gost3411-2012
%{_mandir}/man1/gost3411-2012*

%doc LICENSE README.md

%files devel
%defattr(-, root, root)
%dir %{_includedir}/gost3411-2012
%{_includedir}/gost3411-2012/gost3411-2012-const.h
%{_includedir}/gost3411-2012/gost3411-2012-core.h
%{_includedir}/gost3411-2012/gost3411-2012-mmx.h
%{_includedir}/gost3411-2012/gost3411-2012-precalc.h
%{_includedir}/gost3411-2012/gost3411-2012-ref.h
%{_includedir}/gost3411-2012/gost3411-2012-sse2.h
%{_includedir}/gost3411-2012/gost3411-2012-sse41.h

%changelog
* Wed Apr 25 2018 alexey@renatasystems.org 0.12-1
- Add devel package

* Mon Apr 09 2018 alexey@renatasystems.org 0.12-1
- Initial RPM release

