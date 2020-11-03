import $ from 'jquery';
 function component() {
    const element = document.createElement('pre');

    element.innerHTML = [
      'Hello webpack!',
      '5 cubed is equal to '
    ].join('\n\n');

    return element;
  }

  document.body.appendChild(component());

