window.onload = function() {
    const config = {
        url: "http://127.0.0.1:4221/openapi.json",
        dom_id: '#swagger-ui',
        deepLinking: true,
        presets: [
            SwaggerUIBundle.presets.apis,
            SwaggerUIStandalonePreset
        ],
        plugins: [
            SwaggerUIBundle.plugins.DownloadUrl
        ],
        layout: "StandaloneLayout"
    };

    window.ui = SwaggerUIBundle(config);
};
