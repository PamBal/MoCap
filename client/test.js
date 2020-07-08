const canvas = document.querySelector('#c');
const fov = 100;
const aspect = canvas.clientWidth/canvas.clientHeight;
const near = 0.1;
const far = 1000;

//camera and scene
const scene = new THREE.Scene();
const renderer = new THREE.WebGLRenderer({canvas});
renderer.setClearColor(0xffffff, 1)
const camera = new THREE.PerspectiveCamera(fov, aspect, near, far);
camera.position.z = 4;
camera.position.y = 4;
camera.position.x = 0;
camera.lookAt(0, 0, 0);
//light
const color = 0xFFFFFF;
const intensity = 1;
const light = new THREE.DirectionalLight(color, intensity);
light.position.set(-1, 2, 4);
scene.add(light);

//manipulator object
const shoulderJoint = new THREE.Object3D();
shoulderJoint.position.y = 2;
const shoulderGeometry = new THREE.BoxGeometry(2, 0.1, 0.1);
const shoulderMaterial = new THREE.MeshPhongMaterial({color: 0xaa8844});
const shoulder = new THREE.Mesh(shoulderGeometry, shoulderMaterial);
shoulder.geometry.translate(1, 0, 0);
shoulderJoint.add(shoulder);

const forearmJoint = new THREE.Object3D();
forearmJoint.position.x = 2;
const forearmGeometry = new THREE.BoxGeometry(3, 0.1, 0.1);
const forearmMaterial = new THREE.MeshPhongMaterial({color: 0x8844aa});
const forearm = new THREE.Mesh(forearmGeometry, forearmMaterial);
forearm.geometry.translate(1.5,0,0);
forearmJoint.add(forearm);
shoulderJoint.add(forearmJoint);
scene.add(shoulderJoint);

//plane object
const planeGeometry = new THREE.PlaneGeometry(10, 10);
const planeMaterial = new THREE.MeshPhongMaterial({color: '#0a0'});
const planeMesh = new THREE.Mesh(planeGeometry, planeMaterial);
planeMesh.rotateX(-Math.PI / 2);
planeMesh.receiveShadow = true;
scene.add(planeMesh);

//ball object
const sphereGeometry = new THREE.SphereGeometry();
const sphereMaterial = new THREE.MeshPhongMaterial({color: '#44bcd8'});
const sphereMesh = new THREE.Mesh(sphereGeometry, sphereMaterial);
sphereMesh.receiveShadow = true;
sphereMesh.position.x = 5;
sphereMesh.position.y = 10;
scene.add(sphereMesh);

//world and gravity
const world = new CANNON.World();
world.gravity.set(0, -9.82, 0);

//let cannonDebugRenderer = new THREE.CannonDebugRenderer(scene, world);

// shoulder physics
const shoulderShape = new CANNON.Box(new CANNON.Vec3(2, 0.05, 0.05));
const shoulderBody = new CANNON.Body({mass: 1});
shoulderBody.addShape(shoulderShape);
shoulderBody.position.x = shoulderJoint.position.x;
shoulderBody.position.y = shoulderJoint.position.y;
shoulderBody.position.z = shoulderJoint.position.z;
world.addBody(shoulderBody);

const forearmShape = new CANNON.Box(new CANNON.Vec3(3, 0.05, 0.05));
const forearmBody = new CANNON.Body({mass: 1});
forearmBody.addShape(forearmShape);
let vec = new THREE.Vector3();
let quat = new THREE.Quaternion();
let scale = new THREE.Vector3();
forearmJoint.matrixWorld.decompose(vec, quat, scale);
forearmBody.position.x = vec.x;
forearmBody.position.y = vec.y;
forearmBody.position.z = vec.z;
forearmJoint.quaternion.set(quat.x, quat.y, quat.z, quat.w);
world.add(forearmBody);

//plane physics
const planeShape = new CANNON.Box(new CANNON.Vec3(5, 5, 0.1));
const planeBody = new CANNON.Body({ mass: 0 });
planeBody.addShape(planeShape);
planeBody.quaternion.setFromAxisAngle(new CANNON.Vec3(1, 0, 0), -Math.PI / 2);
world.addBody(planeBody);


//sphere physics
const sphereShape = new CANNON.Sphere(1);
const sphereBody = new CANNON.Body({ mass: 1 });
sphereBody.addShape(sphereShape);
sphereBody.position.x = sphereMesh.position.x;
sphereBody.position.y = sphereMesh.position.y;
sphereBody.position.z = sphereMesh.position.z;
world.addBody(sphereBody);


const clock = new THREE.Clock()

initWS("192.168.4.1:81");
draw();

function draw() {
    requestAnimationFrame(draw);

    if (resizeRendererToDisplaySize(renderer)) {
        const canvas = renderer.domElement;
        camera.aspect = canvas.clientWidth / canvas.clientHeight;
        camera.updateProjectionMatrix();
    }


    let delta = clock.getDelta();
    if (delta > .1) delta = .1;
    if(delta > 0)
        world.step(delta);

    sphereMesh.position.set(sphereBody.position.x, sphereBody.position.y, sphereBody.position.z);
    sphereMesh.quaternion.set(sphereBody.quaternion.x, sphereBody.quaternion.y, sphereBody.quaternion.z, sphereBody.quaternion.w);

    shoulderBody.position.set(shoulderJoint.position.x, shoulderJoint.position.y, shoulderJoint.position.z);
    shoulderBody.quaternion.set(shoulderJoint.quaternion.x, shoulderJoint.quaternion.y, shoulderJoint.quaternion.z, shoulderJoint.quaternion.w)

    forearmJoint.matrixWorld.decompose(vec, quat, scale);
    forearmBody.position.set(vec.x, vec.y, vec.z);
    forearmBody.quaternion.set(quat.x, quat.y, quat.z, quat.w);

   // cannonDebugRenderer.update();
    renderer.render(scene, camera);
}

function resizeRendererToDisplaySize(renderer) {
    const canvas = renderer.domElement;
    const width = canvas.clientWidth;
    const height = canvas.clientHeight;
    const needResize = canvas.width !== width || canvas.height !== height;
    if (needResize) {
        renderer.setSize(width, height, false);
    }
    return needResize;
}

function initWS(addr)
{
    g_webSocket = new WebSocket("ws://" + addr + "/");
    g_webSocket.onopen = function(evt) { console.log("WebSocket open"); };
    g_webSocket.onclose = function(evt) { console.log("WebSocket close"); };
    g_webSocket.onerror = function(evt) { console.log(evt); };
    g_webSocket.onmessage = function(evt) {
        //console.log(evt);

        var msg = JSON.parse(evt.data);
        var q1 = new THREE.Quaternion(msg["q1"][0], msg["q1"][1], msg["q1"][2], msg["q1"][3]);
        var q2 = new THREE.Quaternion(msg["q2"][0], msg["q2"][1], msg["q2"][2], msg["q2"][3]);
        q1.normalize();
        q2.normalize();
        applyMove([q1, q2]);
    };
}

function applyMove(quaternions){
    shoulderJoint.quaternion.copy(quaternions[0]);
    forearmJoint.quaternion.copy(quaternions[1]);
}
