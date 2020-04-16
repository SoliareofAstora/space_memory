import components.model.linear as linear


def get_model(params):
    if params["model_name"] == "linear":
        return linear.create_linear_model(params)