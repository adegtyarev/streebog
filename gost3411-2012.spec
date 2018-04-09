Name:		GOST34.11-2012
Version:	%(cat VERSION)
Release:	1%{?dist}
Summary:	RFC-6986 cryptographic hash function

License:	BSD-2-Clause
URL:		https://streebog.net
Source0:	https://github.com/adegtyarev/streebog/archive/%{version}.tar.gz

BuildRequires:	gcc
BuildRequires:	help2man
ExclusiveArch:  x86_64 i586

%description
Command line tool to create GOST R 34.11-2012 hash digest of files or streams.

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
install -pm 755 gost3411-2012 %{buildroot}%{_bindir}/
install -pm 644 gost3411-2012.1 %{buildroot}%{_mandir}/man1/


%files
%{_bindir}/gost3411-2012
%{_mandir}/man1/gost3411-2012*

%license LICENSE

%doc README.md


%changelog
* Mon Apr 09 2018 alexey@renatasystems.org 0.12-1
- Initial RPM release

