(function () {
    let apbctScript = document.createElement('script');
    apbctScript.type = 'text/javascript';
    apbctScript.async = "true";
    apbctScript.src = 'https://moderate.cleantalk.org/1.1.26/ct-bot-detector.min.js';
    let firstScriptNode = document.getElementsByTagName('script')[0];
    firstScriptNode.parentNode.insertBefore(apbctScript, firstScriptNode);
})();
