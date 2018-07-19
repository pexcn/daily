const test_cases = [
    1, "a.b.c.d.com",
    0, "127.0.0.1",
    0, "qq.com",
    0, "im.qq.com",
    0, "www.imqq.com",
    1, "qqq.com",
    1, "google.com",
    0, "localhost"
];

function isPlainHostName(host) {
    return host.toLowerCase() === "localhost";
}

function dnsResolve(host) {
    return "27.40.0.0";
}

function dnsDomainIs(domain, suffix) {
    return domain.endsWith(suffix);
}

function isInNet(ip, ipstart, ipmask) {
    return false;
}

function shExpMatch(str, shexp) {
    return false;
}

function test(url, host) {
    let ret = FindProxyForURL(url, host);
    if (typeof(direct) === "undefined") {
        if (ret.toLowerCase().indexOf("direct") >= 0) {
            return 0;
        }
        return 1;
    } else if (ret === direct)
        return 0;
    else
        return 1;
}

function output_result(out_obj) {
    output.value = "";
    for (let i = 0; i < test_cases.length; i += 2) {
        const test_case = test_cases[i + 1];
        const test_result = test(test_case, test_case);
        let out_line = "" + test_result + " " + test_case + " ";
        if (test_result === test_cases[i]) {
            out_line = out_line + "Pass";
        } else {
            out_line = out_line + "NOT Pass";
        }
        out_obj.value = out_obj.value + out_line + "\n";
    }
    const start = new Date();
    if (test_cases.length > 1) {
        for (let i = 0; i < 100000; ++i) {
            const test_case = test_cases[1];
            test(test_case, test_case);
        }
    }
    const end = new Date();
    alert(String(end - start) + "ms in 100,000 tests");
}

function begin_test() {
    const output = document.getElementById("output");
    output_result(output);
}

function test_one() {
    const input = document.getElementById("input");
    const result_obj = document.getElementById("result");
    let result = test(input.value, input.value);
    if (result === 1)
        result_obj.value = "Proxy";
    else
        result_obj.value = "Direct";
}
