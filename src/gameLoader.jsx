let scriptPromise = null;
let modulePromise = null;
let moduleInstance = null;

function loadScriptOnce() {
    if (scriptPromise) {
        return scriptPromise;
    }

    scriptPromise = new Promise((resolve, reject) => {
        const script = document.createElement("script");

        const publicUrl = process.env.PUBLIC_URL || '';
        script.src = "${publicUrl}/game/clayborne.js";        
        script.async = true;

        script.onload = () => resolve();
        script.onerror = () => reject(new Error("Failed to load game script"));

        document.body.appendChild(script);
    });

    return scriptPromise;
}

export async function initGame(canvas) {
    if (moduleInstance) {
        return moduleInstance;
    }

    await loadScriptOnce();

    if (!window.clayborneModule) {
        throw new Error("Emscripten module not found");
    }

    if (!modulePromise) {
        modulePromise = window.clayborneModule({
            canvas
        }).then((mod) => {
            moduleInstance = mod;
            return mod;
        });
    }

    return modulePromise;
}

export function getGameInstance() {
    return moduleInstance;
}