/*
 * vim: set ft=rust:
 * vim: set ft=reason:
 */

let start () => {
  let open Js.Unsafe;
  let renderer = Repixi.autoDetectRenderer width::800 height::600;
  Repixi.Dom.appendToBody renderer#view;
  let stage = new Repixi.Container.t;
  let loader = Repixi.Loader.add name::"pixie" uri::"_assets/spine/Pixie.json";
  stage#setInteractive true;
  loader#load (
    fun loader data => {
      let background = Repixi.Sprite.fromImage uri::"_assets/spine/iP4_BGtile.jpg";
      let background2 = Repixi.Sprite.fromImage uri::"_assets/spine/iP4_BGtile.jpg";
      stage#addChild background;
      stage#addChild background2;
      let forground = Repixi.Sprite.fromImage uri::"_assets/spine/iP4_ground.png";
      let forground2 = Repixi.Sprite.fromImage uri::"_assets/spine/iP4_ground.png";
      stage#addChild forground;
      stage#addChild forground2;
      let (x1, _) = forground#position;
      forground#setPosition (x1, 640 - forground2#height);
      let (x2, _) = forground2#position;
      forground2#setPosition (x2, 640 - forground2#height);
      let pixie = (new Repixi.Spine.t) data;
      pixie#setPosition (1024 / 3, 500);
      pixie#setScale (0.3, 0.3);
      stage#addChildSpine pixie;
      pixie#setMixByName from::"running" too::"jump" duration::0.2;
      pixie#setMixByName from::"jump" too::"running" duration::0.4;
      pixie#setAnimationByName startTime::0 animName::"running" loop::true;
      let onActionStart _ => {
        pixie#setAnimationByName startTime::0 animName::"jump" loop::false;
        pixie#addAnimationByName startTime::0 animName::"running" loop::true randomInt::0
      };
      stage#on Repixi.Events.MouseDown onActionStart;
      stage#on Repixi.Events.TouchStart onActionStart;
      let rec animate position () => {
        let position = position + 10;
        let positionf = float_of_int position;
        let newX = ref ((- (int_of_float (positionf *. 0.6))) mod (1286 * 2));
        if (!newX < 0) {
          newX := !newX + 1286 * 2
        };
        let (_, y) = background#position;
        background#setPosition (!newX - 1286, y);
        let newX = ref (((- (int_of_float (positionf *. 0.6))) + 1286) mod (1286 * 2));
        if (!newX < 0) {
          newX := !newX + 1286 * 2
        };
        let (_, y) = background2#position;
        background2#setPosition (!newX - 1286, y);
        let newX = ref ((- position) mod (1286 * 2));
        if (!newX < 0) {
          newX := !newX + 1286 * 2
        };
        let (_, y) = forground#position;
        forground#setPosition (!newX - 1286, y);
        let newX = ref (((- position) + 1286) mod (1286 * 2));
        if (!newX < 0) {
          newX := !newX + 1286 * 2
        };
        let (_, y) = forground2#position;
        forground2#setPosition (!newX - 1286, y);
        Dom_html._requestAnimationFrame (Js.wrap_callback (animate position));
        renderer#render stage
      };
      animate 0 ()
    }
  )
};
