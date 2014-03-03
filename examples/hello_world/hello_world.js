(function() {
  'use strict';

  var module = null;
  var isLoaded = false;
  var statusText = 'NO-STATUS';

  // Set the global status message.  If the element with id 'statusField'
  // exists, then set its HTML to the status message as well.
  // opt_message The message test.  If this is null or undefined, then
  // attempt to set the element with id 'statusField' to the value of
  // |statusText|.
  function updateStatus(opt_message) {
    if (opt_message)
      statusText = opt_message;
    var statusField = document.getElementById('statusField');
    if (statusField) {
      statusField.innerHTML = statusText;
    }
  }

  // Indicate load success.
  function moduleDidLoad() {
    isLoaded = true;
    updateStatus('LOADED');
  }

  // If the page loads before the Native Client module loads, then set the
  // status message indicating that the module is still loading.  Otherwise,
  // do not change the status message.
  var onLoad = function () {
    var naclElement = createNaclElement('nacl-module',
                                        'pnacl/Release/hello_world.nmf');
    naclElement.addEventListener('load', moduleDidLoad, true);
    document.body.appendChild(naclElement);

    module = new NaClModule(naclElement);

    updateStatus('LOADING...');
  };

  var onFormSubmit = function () {
    if (!isLoaded) {
      alert('NaCl module not loaded');
      return false;
    }

    var nameElement = document.getElementById('name');
    var data = {
        name: nameElement.value
    };

    var onSuccess = function (response) {
      alert(response.result);  
    };

    var onError = function (error) {
      alert('Error: ' + error.message);
    };

    module.postMessage('testMessage', data, onSuccess, onError);
    return false;
  };

  window.onLoad = onLoad;
  window.onFormSubmit = onFormSubmit;
})();
