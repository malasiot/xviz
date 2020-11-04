import React, { Component } from "react";
import ReactDOM from "react-dom";

import PinchZoomPan from "react-responsive-pinch-zoom-pan";

const App = () => {
    return (
        <div style={{ width: '500px', height: '500px' }}>
            <PinchZoomPan>
		<h1>Hello</h1>
             
            </PinchZoomPan>
        </div>
    );
};

const wrapper = document.getElementById("container");
wrapper ? ReactDOM.render(<App />, wrapper) : false;

