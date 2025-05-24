let initialTilt = null;

let scene, camera, renderer, model, controls;
let mqttClient;
let isAnimating = false;
let targetTilt = 0;

function init() {
    // Configuración inicial de Three.js
    scene = new THREE.Scene();
    scene.background = new THREE.Color(0xF0F0F0);

    camera = new THREE.PerspectiveCamera(80, window.innerWidth / window.innerHeight, 0.1, 500);
    camera.position.set(-5, 2, -1);
    camera.lookAt(0, 0, 0);

    renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(400, 650);
    document.getElementById('3Dmodel').appendChild(renderer.domElement);

    controls = new THREE.OrbitControls(camera, renderer.domElement);
    controls.enableDamping = false;

    // Configuración de luces
    const light = new THREE.AmbientLight(0xffffff, 1);
    scene.add(light);
    const hemiLight = new THREE.HemisphereLight(0xffffff, 0x444444, 1);
    scene.add(hemiLight);
    const dirLight = new THREE.DirectionalLight(0xffffff, 1);
    dirLight.position.set(5, 5, 5);
    scene.add(dirLight);

    // Cargar modelo 3D
    const loader = new THREE.FBXLoader();
    loader.load('motorexp.fbx', (object) => {
        model = object;
        model.scale.set(0.6, 0.6, 0.6);
        model.position.set(0.5, -2.5, 0);
        scene.add(model);
        console.log('Modelo cargado correctamente');
    }, undefined, (error) => {
        console.error('Error al cargar el modelo:', error);
    });

    // Configurar slider
    const slider = document.getElementById('tiltSlider');
    slider.addEventListener('input', (event) => {
        if (isAnimating) return;
        const value = parseInt(event.target.value);
        updateModelTilt(value);
        updateSliderColor(value);
    });

    connectMQTT();
    animate();
}

function updateModelTilt(value) {
    if (model) {
        model.rotation.x = value * Math.PI / 180;
    }
}

function updateSliderColor(value) {
    const slider = document.getElementById('tiltSlider');
    const valueText = document.getElementById("sliderValue");
    const root = document.documentElement;

    let color = 'red';
    if (value >= -20 && value <= 20) color = "green";
    else if ((value >= 21 && value <= 45) || (value <= -21 && value >= -45)) color = "orange";

    const percentage = ((value + 90) / 180) * 100;
    root.style.setProperty('--slider-color', color);
    root.style.setProperty('--slider-percentage', `${percentage}%`);
    slider.value = value;
    valueText.textContent = value;
    valueText.style.color = color;
}

function connectMQTT() {
    const host = "ws://test.mosquitto.org:8080/mqtt";
    mqttClient = new Paho.MQTT.Client(host, "cliente_web_" + Math.random().toString(16).substr(2, 8));

    mqttClient.onConnectionLost = (responseObject) => {
        console.log("❌ Conexión perdida:", responseObject.errorMessage);
    };

    mqttClient.onMessageArrived = (message) => {
    if (isAnimating) return;

    try {
        const msg = JSON.parse(message.payloadString);
        if (msg?.pitch !== undefined) {
            const rawPitch = parseInt(msg.pitch);

            // Guardar el primer valor como base
            if (initialTilt === null) {
                initialTilt = rawPitch;
                console.log("Valor inicial registrado como 0:", initialTilt);
            }

            const tiltValue = rawPitch - initialTilt;

            if (tiltValue === 45 || tiltValue === -45) {
                targetTilt = tiltValue;
                startTiltAnimation();
            } else {
                updateModelTilt(tiltValue);
                updateSliderColor(tiltValue);
            }
        }
    } catch (e) {
        console.error('Error procesando mensaje:', e);
    }
};


    mqttClient.connect({
        onSuccess: () => {
            console.log("✅ Conectado a MQTT");
            mqttClient.subscribe("RS/state");
        },
        onFailure: (error) => {
            console.log("❌ Error de conexión:", error);
        }
    });
}

function startTiltAnimation() {
    if (isAnimating) return;
    
    isAnimating = true;
    const startValue = parseFloat(document.getElementById('tiltSlider').value);
    const duration = 80;
    const startTime = performance.now();

    const animateFrame = () => {
        const elapsed = performance.now() - startTime;
        const progress = Math.min(elapsed / duration, 1);
        const easedProgress = easeInOutQuad(progress);
        const currentValue = startValue + (targetTilt - startValue) * easedProgress;

        updateModelTilt(currentValue);
        updateSliderColor(currentValue);

        if (progress < 1) {
            requestAnimationFrame(animateFrame);
        } else {
            isAnimating = false;
        }
    };

    requestAnimationFrame(animateFrame);
}

function easeInOutQuad(t) {
    return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

function animate() {
    requestAnimationFrame(animate);
    controls.update();
    renderer.render(scene, camera);
}

window.onload = init;