[@bs.config {jsx: 3}];
[%bs.raw {|require("./QuestionsShow.css")|}];

open QuestionsShow__Types;

let str = React.string;

type action =
  | AddComment(Comment.t)
  | AddAnswer(Answer.t, bool)
  | AddLike(Like.t)
  | RemoveLike(string)
  | UpdateShowAnswerCreate(bool)
  | UpdateShowQuestionEdit(bool)
  | UpdateQuestion(Question.t)
  | UpdateAnswer(Answer.t)
  | UpdateComment(Comment.t);

type state = {
  question: Question.t,
  answers: list(Answer.t),
  comments: list(Comment.t),
  likes: list(Like.t),
  showAnswerCreate: bool,
  showQuestionEdit: bool,
};

let reducer = (state, action) =>
  switch (action) {
  | AddComment(comment) => {
      ...state,
      comments: Comment.addComment(state.comments, comment),
    }
  | AddAnswer(answer, bool) => {
      ...state,
      answers: Answer.addAnswer(state.answers, answer),
      showAnswerCreate: bool,
    }

  | AddLike(like) => {...state, likes: state.likes |> Like.addLike(like)}
  | RemoveLike(id) => {...state, likes: state.likes |> Like.removeLike(id)}
  | UpdateShowAnswerCreate(bool) => {...state, showAnswerCreate: bool}
  | UpdateShowQuestionEdit(bool) => {...state, showQuestionEdit: bool}
  | UpdateQuestion(question) => {...state, question, showQuestionEdit: false}
  | UpdateAnswer(answer) => {
      ...state,
      answers: Answer.updateAnswer(state.answers, answer),
    }
  | UpdateComment(comment) => {
      ...state,
      comments: Comment.updateComment(state.comments, comment),
    }
  };

let showAnswersCreateComponent = (answers, showAnswerCreate, currentUserId) =>
  if (showAnswerCreate) {
    true;
  } else {
    answers |> Answer.answerFromUser(currentUserId) |> ListUtils.isEmpty;
  };

let likesForAnswer = (likes, answerId) =>
  likes |> Like.likesForAnswer(answerId) |> List.length;

let handleUpdateQuestion =
    (title, description, currentUserId, question, dispatch) => {
  let newQuestion =
    Question.create(
      question |> Question.id,
      title,
      description,
      question |> Question.creatorId,
      Some(currentUserId),
      question |> Question.createdAt,
    );
  dispatch(UpdateQuestion(newQuestion));
};

let handleUpdateAnswer = (id, answers, dispatch) => {
  let oldAnswer = answers |> Answer.findAnswer(id);
  let newAnswer =
    Answer.create(
      id,
      oldAnswer |> Answer.description,
      oldAnswer |> Answer.creatorId,
      oldAnswer |> Answer.editorId,
      oldAnswer |> Answer.createdAt,
      true,
    );
  dispatch(UpdateAnswer(newAnswer));
};

let handleUpdateComment = (id, comments, dispatch) => {
  let oldComment = comments |> Comment.findComment(id);
  let newComment =
    Comment.create(
      oldComment |> Comment.id,
      oldComment |> Comment.value,
      oldComment |> Comment.creatorId,
      oldComment |> Comment.commentableId,
      oldComment |> Comment.commentableType,
      true,
    );

  dispatch(UpdateComment(newComment));
};

[@react.component]
let make =
    (
      ~authenticityToken,
      ~question,
      ~answers,
      ~comments,
      ~userData,
      ~likes,
      ~currentUserId,
      ~communityPath,
      ~isCoach,
      ~communityId,
    ) => {
  let (state, dispatch) =
    React.useReducer(
      reducer,
      {
        question,
        answers,
        comments,
        likes,
        showAnswerCreate: false,
        showQuestionEdit: false,
      },
    );
  let addCommentCB = comment => dispatch(AddComment(comment));
  let handleAnswerCB = (answer, newAnswer) =>
    newAnswer ?
      dispatch(AddAnswer(answer, false)) : dispatch(UpdateAnswer(answer));
  let addLikeCB = like => dispatch(AddLike(like));
  let removeLikeCB = id => dispatch(RemoveLike(id));
  let updateQuestionCB = (title, description) =>
    handleUpdateQuestion(
      title,
      description,
      currentUserId,
      question,
      dispatch,
    );
  let archiveCB = (id, resourceType) =>
    switch (resourceType) {
    | "Question" =>
      communityPath |> Webapi.Dom.Window.setLocation(Webapi.Dom.window)
    | "Answer" => handleUpdateAnswer(id, state.answers, dispatch)
    | "Comment" => handleUpdateComment(id, state.comments, dispatch)
    | _ =>
      Notification.error(
        "Something went wrong",
        "Please refresh the page and try again",
      )
    };
  let filteredAnswers =
    state.answers |> List.filter(answer => !(answer |> Answer.archived));

  <div className="flex flex-1 bg-gray-100">
    <div className="flex-1 flex flex-col">
      <div className="flex-col px-3 md:px-6 py-2 items-center justify-between">
        {
          state.showQuestionEdit ?
            <div>
              <div className="max-w-2xl w-full mx-auto mt-5 pb-2 text-right">
                <a
                  id="close-button"
                  className="btn btn-default no-underline"
                  onClick={
                    event => {
                      event |> ReactEvent.Mouse.preventDefault;
                      dispatch(UpdateShowQuestionEdit(false));
                    }
                  }>
                  <i className="far fa-arrow-left" />
                  <span className="ml-2"> {"close" |> str} </span>
                </a>
              </div>
              <QuestionsEditor
                authenticityToken
                communityId
                target=None
                question={state.question}
                updateQuestionCB
              />
            </div> :
            <div>
              <div className="max-w-3xl w-full mx-auto mt-5 pb-2">
                <a className="btn btn-default no-underline" href=communityPath>
                  <i className="far fa-arrow-left" />
                  <span className="ml-2"> {React.string("Back")} </span>
                </a>
              </div>
              <div
                className="max-w-3xl w-full flex mx-auto items-center justify-center relative shadow bg-white border rounded-lg">
                <div className="flex w-full">
                  <div className="flex flex-1 flex-col">
                    <div className="pt-6 pb-2 mx-6 flex flex-col">
                      <h2 className="text-xl text-black font-semibold">
                        {state.question |> Question.title |> str}
                      </h2>
                    </div>
                    <div className="py-4 px-6 flex flex-col">
                      <div
                        className="leading-normal text-sm markdown-body"
                        dangerouslySetInnerHTML={
                          "__html": state.question |> Question.description,
                        }
                      />
                      {
                        state.question
                        |> Question.creatorId == currentUserId
                        || isCoach ?
                          <div>
                            <a
                              onClick={
                                _ => dispatch(UpdateShowQuestionEdit(true))
                              }
                              className="text-sm mr-2 font-semibold cursor-pointer">
                              {"Edit" |> str}
                            </a>
                            <QuestionsShow__ArchiveManager
                              authenticityToken
                              id={question |> Question.id}
                              resourceType="Question"
                              archiveCB
                            />
                          </div> :
                          React.null
                      }
                    </div>
                    <div className="flex flex-row justify-between px-6 pb-6">
                      <div className="pr-2 pt-6 text-center">
                        <i
                          className="fal fa-comment-lines text-xl text-gray-600"
                        />
                        <p className="text-xs py-1">
                          {
                            state.comments
                            |> Comment.commentsForQuestion
                            |> List.length
                            |> string_of_int
                            |> str
                          }
                        </p>
                      </div>
                      <QuestionsShow__UserShow
                        userProfile={
                          userData
                          |> UserData.user(
                               state.question |> Question.creatorId,
                             )
                        }
                        createdAt={state.question |> Question.createdAt}
                        textForTimeStamp="Asked"
                      />
                    </div>
                  </div>
                </div>
              </div>
              <QuestionsShow__CommentShow
                comments={state.comments |> Comment.commentsForQuestion}
                userData
                authenticityToken
                commentableType="Question"
                commentableId={state.question |> Question.id}
                addCommentCB
                currentUserId
                archiveCB
                isCoach
              />
              <div
                className="max-w-3xl w-full justify-center mx-auto mb-4 pt-5 pb-2 border-b">
                <div className="flex items-end">
                  <span className="text-lg font-semibold">
                    {
                      (state.answers |> List.length |> string_of_int)
                      ++ " Answers"
                      |> str
                    }
                  </span>
                  {
                    let archivedAnswerCount =
                      (state.answers |> List.length)
                      - (filteredAnswers |> List.length);
                    archivedAnswerCount == 0 ?
                      React.null :
                      <span className="text-xs font-normal px-2">
                        {
                          "( "
                          ++ (archivedAnswerCount |> string_of_int)
                          ++ " hidden)"
                          |> str
                        }
                      </span>;
                  }
                </div>
              </div>
              <div className="community-answer-container">
                {
                  filteredAnswers
                  |> List.map(answer =>
                       (answer, likesForAnswer(likes, answer |> Answer.id))
                     )
                  |> List.sort(((_, likeX), (_, likeY)) => likeY - likeX)
                  |> List.map(((answer, _)) =>
                       <QuestionsShow__AnswerShow
                         key={answer |> Answer.id}
                         authenticityToken
                         answer
                         question={state.question}
                         addCommentCB
                         currentUserId
                         addLikeCB
                         removeLikeCB
                         userData
                         comments={state.comments}
                         likes={state.likes}
                         handleAnswerCB
                         isCoach
                         archiveCB
                       />
                     )
                  |> Array.of_list
                  |> ReasonReact.array
                }
              </div>
              {
                showAnswersCreateComponent(
                  state.answers,
                  state.showAnswerCreate,
                  currentUserId,
                ) ?
                  <QuestionsShow__AnswerEditor
                    question={state.question}
                    authenticityToken
                    currentUserId
                    handleAnswerCB
                  /> :
                  <div
                    className="community-ask-button-container mt-4 my-8 max-w-3xl w-full flex mx-auto justify-center">
                    <div className="bg-gray-100 px-1 z-10">
                      <button
                        className="btn btn-primary btn-large"
                        onClick={_ => dispatch(UpdateShowAnswerCreate(true))}>
                        {"Add another answer" |> str}
                      </button>
                    </div>
                  </div>
              }
            </div>
        }
      </div>
    </div>
  </div>;
};