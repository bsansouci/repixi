/*
 * vim: set ft=rust:
 * vim: set ft=reason:
 */

let start () => {
  let open Js.Unsafe;
  Random.self_init ();
  let renderer = Repixi.autoDetectRendererWithParam width::800 height::600 param::{
    backgroundColor: 0x1099bb
  };
  Repixi.Dom.appendToBody renderer#view;
  let stage = new Repixi.Container.t;
  let container = new Repixi.Container.t;
  stage#addChildContainer(container);
  for j in 0 to 4 {
    for i in 0 to 4 {
      let bunny = Repixi.Sprite.fromImage uri::"_assets/basics/bunny.png";
      bunny#setPosition ((30 * i), (30 * j));
      bunny#setRotation (Random.float 1. *. Repixi.pi);
      container#addChild bunny;
    };
  };
  let rt = new Repixi.RenderTexture.t sampleRatio::0.1 300 200;
  let sprite = new Repixi.Sprite.t rt;
  sprite#setPosition (450, 60);
  stage#addChild sprite;
  container#setPosition (100, 60);

  let rec animate () => {
    Dom_html._requestAnimationFrame (Js.wrap_callback animate);
    renderer#renderToTexture container rt;
    renderer#render stage;
  };
  animate ();
};
