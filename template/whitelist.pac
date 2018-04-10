var proxy = "SOCKS5 127.0.0.1:1080;";
var direct = "DIRECT;";

var china_domains = {
__CHINA_DOMAIN_LIST_PLACEHOLDER__
};

var subnet_ips = [
0,1,                     // 0.0.0.0
167772160,184549376,     // 10.0.0.0/8
-1408237568,-1407188992, // 172.16.0.0/12
-1062731776,-1062666240, // 192.168.0.0/16
1681915904,1686110208,   // 100.64.0.0/10
2130706432,2130706688    // 127.0.0.0/24
]

var hasOwnProperty = Object.hasOwnProperty;

function is_ipv4(host) {
    var regex = /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])(:([0-9]|[1-9]\d|[1-9]\d{2}|[1-9]\d{3}|[1-5]\d{4}|6[0-4]\d{3}|65[0-4]\d{2}|655[0-2]\d|6553[0-5]))?$/g;
    if (regex.test(host)) {
        return true;
    }
}

function convert_address(ipchars) {
    if (ipchars.indexOf(':') !== -1) {
        ipchars = ipchars.split(':')[0];
    }
    var bytes = ipchars.split('.');
    var result = ((bytes[0] & 0xff) << 24) |
                 ((bytes[1] & 0xff) << 16) |
                 ((bytes[2] & 0xff) <<  8) |
                 (bytes[3] & 0xff);
    return result;
}

function is_china_domain(domain) {
    return !!(dnsDomainIs(domain, ".cn") || dnsDomainIs(domain, ".com.cn") || dnsDomainIs(domain, ".gov.cn"));
}

function match_domains(domain, domains) {
    var suffix;
    var pos = domain.lastIndexOf('.');
    pos = domain.lastIndexOf('.', pos - 1);
    while(1) {
        if (pos <= 0) {
            if (hasOwnProperty.call(domains, domain)) {
                return true;
            } else {
                return false;
            }
        }
        suffix = domain.substring(pos + 1);
        if (hasOwnProperty.call(domains, suffix)) {
            return true;
        }
        pos = domain.lastIndexOf('.', pos - 1);
    }
}

function match_subnet_ips(ip, ips) {
    for (var i = 0; i < 12; i += 2) {
        if (ips[i] <= ip && ip < ips[i + 1]) {
            return true;
        }
    }
}

function FindProxyForURL(url, host) {
    if (typeof host === 'undefined'
        || isPlainHostName(host) === true
        || host === '127.0.0.1'
        || host === 'localhost') {
        return direct;
    }

    if (is_china_domain(host) === true) {
        return direct;
    }

    if (match_domains(host, china_domains) === true) {
        return direct;
    }

    if (is_ipv4(host) === true) {
        var ip = convert_address(host);
        if (match_subnet_ips(ip, subnet_ips) === true) {
            return direct;
        }
    }

    return proxy;
}
