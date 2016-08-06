const PIXI = require('PIXI');

const requestAnimationFrame = require('requestAnimationFrame');

var renderer = PIXI.autoDetectRenderer(800, 600, {
  backgroundColor: 0x1099bb
});
document.body.appendChild(renderer.view);
var stage = new PIXI.Container();
var container = new PIXI.Container();
stage.addChild(container);
for (var j = 0; j < 5; j++) {
  for (var i = 0; i < 5; i++) {
    var bunny = PIXI.Sprite.fromImage('_assets/basics/bunny.png');
    bunny.x = 30 * i;
    bunny.y = 30 * j;
    bunny.rotation = Math.random() * (Math.PI * 2)
    container.addChild(bunny);
  };
}
var rt = new PIXI.RenderTexture(renderer, 300, 200, PIXI.SCALE_MODES.LINEAR, 0.1);
var sprite = new PIXI.Sprite(rt);
sprite.x = 450;
sprite.y = 60;
stage.addChild(sprite)
container.x = 100;
container.y = 60;
animate();

function animate() {
  rt.render(container)
  requestAnimationFrame(animate);
  renderer.render(stage);
}
