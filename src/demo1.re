/* --------------Demo code---------------- */

let start () => {
  let renderer = Renderer.autoDetectRenderer width::800 height::600;
  ignore (
    Js.Unsafe.meth_call
      (Js.Unsafe.get Dom_html.document "body") "appendChild" [|Js.Unsafe.inject renderer#view|]
  );
  let stage = Repixi.createContainer ();
  Repixi.loadTexture
    callback::(
      fun loader resources => {
        let open Js.Unsafe;
        let bunny = Repixi.createSprite texture::(get (get resources "bunny") "texture");
        set (get bunny "position") "x" 400;
        set (get bunny "position") "y" 300;
        set (get bunny "scale") "x" 2;
        set (get bunny "scale") "y" 2;
        ignore @@ meth_call stage "addChild" [|inject bunny|];
        let rec animate () => {
          let open Js.Unsafe;
          Dom_html._requestAnimationFrame (Js.wrap_callback animate);
          set bunny "rotation" (Js.to_float (get bunny "rotation") +. 0.01);
          renderer#render stage;
        };
        animate ()
      }
    )
    name::"bunny"
    uri::"_assets/bunny.png"
};
