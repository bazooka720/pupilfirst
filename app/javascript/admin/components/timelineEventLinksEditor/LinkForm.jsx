import React from "react";
import PropTypes from "prop-types";

export default class LinkForm extends React.Component {
  constructor(props) {
    super(props);

    let initialLink = null;

    if (!props.link) {
      //if no links received, initialize an empty object so that we can still call link.title etc
      initialLink = { title: "", url: "", private: false };
    } else {
      initialLink = props.link;
    }

    this.state = { link: initialLink, titleError: false, urlError: false };

    this.saveLink = this.saveLink.bind(this);
    this.handleInputChange = this.handleInputChange.bind(this);
    this.clearErrorMarkers = this.clearErrorMarkers.bind(this);
  }

  //updates the open form if user changes its role (by clicking edit on another link for eg)
  componentWillReceiveProps(newProps) {
    this.setState({ link: newProps.link });
  }

  handleInputChange(event) {
    //copy links to a new object to avoid pass by reference
    var presentLink = $.extend({}, this.state.link);

    switch (event.target.id) {
      case "link_title":
        presentLink.title = event.target.value;
        break;
      case "link_url":
        presentLink.url = event.target.value;
        break;
      case "link_private":
        presentLink.private = event.target.checked;
        break;
    }
    this.setState({ link: presentLink });
  }

  saveLink() {
    var new_title = $("#link_title").val();
    var new_url = $("#link_url").val();
    var new_private = $("#link_private").prop("checked");

    if (new_title && this.isUrlValid(new_url)) {
      if (!this.linkProvided()) {
        this.props.linkAddedCB({
          title: new_title,
          url: new_url,
          private: new_private
        });
      } else {
        this.props.editLinkCB({
          title: new_title,
          url: new_url,
          private: new_private,
          index: this.state.link.index
        });
      }
    } else {
      if (!new_title) {
        this.setState({ titleError: true });
      }
      if (!this.isUrlValid(new_url)) {
        this.setState({ urlError: true });
      }
    }
  }

  isUrlValid(url) {
    return /^(https?|s?ftp):\/\/(((([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:)*@)?(((\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])\.(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5]))|((([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))\.)+(([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))\.?)(:\d*)?)(\/((([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)+(\/(([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)*)*)?)?(\?((([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)|[\uE000-\uF8FF]|\/|\?)*)?(#((([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(%[\da-f]{2})|[!\$&'\(\)\*\+,;=]|:|@)|\/|\?)*)?$/i.test(
      url
    );
  }

  //upon focus, clear error markers, if any
  clearErrorMarkers(event) {
    if (event.target.id === "link_title") {
      this.setState({ titleError: false });
    } else if (event.target.id === "link_url") {
      this.setState({ urlError: false });
    }
  }

  linkProvided() {
    //link.index will be 'undefined' for 'Add Link'
    return _.isNumber(this.state.link.index);
  }

  render() {
    return (
      <div>
        <fieldset className="inputs">
          <legend>
            {this.linkProvided() ? (
              <span>Edit Link</span>
            ) : (
              <span>Add Link</span>
            )}
          </legend>
          <ol>
            <span style={{ color: "red", paddingLeft: "10px" }}>
              Make sure you save this link before clicking{" "}
              <em>
                <strong>Update Timeline Event</strong>
              </em>.
            </span>

            <li className="text input required">
              <label htmlFor="link_title" className="label">
                Title
              </label>
              <input
                id="link_title"
                name="link_title"
                type="text"
                placeholder="(required)"
                value={this.state.link.title}
                onFocus={this.clearErrorMarkers}
                onChange={this.handleInputChange}
              />
              {this.state.titleError && (
                <em className="aa-error-hint">That doesn't look right.</em>
              )}
            </li>

            <li className="text input required">
              <label htmlFor="link_url" className="label">
                URL
              </label>
              <input
                id="link_url"
                name="link_url"
                type="text"
                placeholder="(required)"
                value={this.state.link.url}
                onFocus={this.clearErrorMarkers}
                onChange={this.handleInputChange}
              />
              {this.state.urlError && (
                <em className="aa-error-hint">
                  Please make sure you've supplied a full URL, starting with
                  http(s).
                </em>
              )}
            </li>

            <li className="boolean input optional">
              <label htmlFor="link_private">
                <input
                  id="link_private"
                  type="checkbox"
                  name="link_private"
                  checked={this.state.link.private}
                  onChange={this.handleInputChange}
                />
                Hide from public?
              </label>
            </li>
          </ol>
        </fieldset>

        <fieldset className="actions">
          <ol>
            <li className="action input_action">
              <a onClick={this.saveLink} className="button">
                <i className="fa fa-save" /> Save Link
              </a>
            </li>
          </ol>
        </fieldset>
      </div>
    );
  }
}

LinkForm.propTypes = {
  link: PropTypes.object,
  linkAddedCB: PropTypes.func.isRequired,
  editLinkCB: PropTypes.func.isRequired
};
